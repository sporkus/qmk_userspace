package main

import (
	"fmt"
	"os"
	"strings"

	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
)

var (
	titleStyle = lipgloss.NewStyle().
			Bold(true).
			Foreground(lipgloss.Color("#FFA500")).
			PaddingBottom(0)

	statusActive = lipgloss.NewStyle().
			Foreground(lipgloss.Color("#33FF33")).
			Bold(true)

	statusInactive = lipgloss.NewStyle().
			Foreground(lipgloss.Color("#888888"))

	tableHeaderStyle = lipgloss.NewStyle().
				Foreground(lipgloss.Color("#AAAAAA")).
				Bold(true)

	helpStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("#666666"))

	greenStyle = lipgloss.NewStyle().Foreground(lipgloss.Color("#33FF33"))
	faintStyle = lipgloss.NewStyle().Foreground(lipgloss.Color("#555555"))
)

type Model struct {
	state     ECState
	lines     chan string
	err       string
	width     int
}

func (m Model) Init() tea.Cmd {
	return waitForLine(m.lines)
}

type lineMsg string

func (m Model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.WindowSizeMsg:
		m.width = msg.Width
		return m, nil

	case lineMsg:
		ParseLine(strings.TrimSpace(string(msg)), &m.state)
		// Drain any lines already buffered so a burst of EC_ADC rows
		// becomes a single screen redraw instead of one per row.
		for {
			select {
			case line, ok := <-m.lines:
				if !ok {
					return m, tea.Quit
				}
				ParseLine(strings.TrimSpace(line), &m.state)
			default:
				return m, waitForLine(m.lines)
			}
		}

	case tea.KeyMsg:
		if msg.Type == tea.KeyCtrlC {
			return m, tea.Quit
		}
	}
	return m, nil
}

func (m Model) View() string {
	if m.err != "" {
		return lipgloss.NewStyle().Foreground(lipgloss.Color("#FF4444")).Render(m.err)
	}

	title := titleStyle.Render(" EC Calibration ")

	if !m.state.Ready {
		status := statusInactive.Render("○  press EC_TUI on keyboard to connect")
		bar := lipgloss.JoinHorizontal(lipgloss.Center, title, "  ", status)
		help := helpStyle.Render("\n  Ctrl+C to quit")
		return bar + "\n" + help + "\n"
	}

	s := &m.state
	cols := s.Config.Cols

	// --- Header ---
	var status string
	switch {
	case s.CalPhase == 1:
		status = statusActive.Render("● PHASE 1: lift all fingers — tuning baseline…")
	case s.CalPhase == 2:
		status = statusActive.Render("● PHASE 2: bottom every key, then press EC_CAL to save")
	case s.TuiMode:
		status = statusInactive.Render("○ TUI active  — press EC_CAL to calibrate")
	default:
		status = statusInactive.Render("○ press EC_TUI on keyboard to stream data")
	}
	bar := lipgloss.JoinHorizontal(lipgloss.Center, title, "  ", status)

	// --- Config line ---
	gamma := s.Config.Gamma
	if gamma <= 0 {
		gamma = 1
	}
	configLine := helpStyle.Render(fmt.Sprintf(
		"actuation: %d%%  release: %d%%  curve gamma: %.2g  (1.0 = linear, higher = more physical travel per %% actuation)",
		s.Config.ActuationOffset, s.Config.ReleaseOffset, gamma,
	))

	// --- Bottoming table ---
	bottoming := renderBottomingTable(s, cols)

	// --- ADC table ---
	adcTable := renderADCTable(s, cols)

	// --- Travel% table ---
	travelTable := renderTravelTable(s, cols)

	help := helpStyle.Render("Ctrl+C to quit  ·  EC_TUI = toggle stream  ·  EC_CAL = toggle calibration")

	return strings.Join([]string{
		bar,
		configLine,
		"",
		bottoming,
		"",
		adcTable,
		"",
		travelTable,
		"",
		help,
	}, "\n")
}

// rowLabelWidth is the character width of the leading row-label cells.
const rowLabelWidth = 6

// colCellWidth defines the fixed width of a data cell in tables.
const (
	bottomingCellWidth = 6
	adcCellWidth       = 6
	travelCellWidth    = 7
)

func colHeaders(cols int, cellWidth int) string {
	parts := make([]string, cols)
	for c := 0; c < cols; c++ {
		parts[c] = fmt.Sprintf("%*s", cellWidth, fmt.Sprintf("C%d", c))
	}
	return strings.Join(parts, "")
}

func renderBottomingTable(s *ECState, cols int) string {
	line0 := tableHeaderStyle.Render("[Bottoming]")
	line1 := strings.Repeat(" ", rowLabelWidth) + colHeaders(cols, bottomingCellWidth)
	lines := make([]string, 2, s.Config.Rows+2)
	lines[0] = line0
	lines[1] = line1

	for r := 0; r < s.Config.Rows; r++ {
		rowStr := fmt.Sprintf("  R%d  ", r)
		for c := 0; c < cols; c++ {
			cal := s.Cal[r][c]
			if cal.Idle == 0 && cal.Bottom == 0 {
				rowStr += fmt.Sprintf("%*s", bottomingCellWidth, "✗")
			} else if s.KeyCalibrated(r, c) {
				rowStr += greenStyle.Render(fmt.Sprintf("%*d", bottomingCellWidth, cal.Bottom))
			} else {
				rowStr += faintStyle.Render(fmt.Sprintf("%*d", bottomingCellWidth, cal.Bottom))
			}
		}
		lines = append(lines, rowStr)
	}
	return strings.Join(lines, "\n")
}

func renderADCTable(s *ECState, cols int) string {
	line0 := tableHeaderStyle.Render("[ADC values]")
	line1 := strings.Repeat(" ", rowLabelWidth) + colHeaders(cols, adcCellWidth)
	lines := make([]string, 2, s.Config.Rows+2)
	lines[0] = line0
	lines[1] = line1

	for r := 0; r < s.Config.Rows; r++ {
		rowStr := fmt.Sprintf("  R%d  ", r)
		for c := 0; c < cols; c++ {
			live := s.Live[r][c]
			if !live.valid {
				rowStr += fmt.Sprintf("%*s", adcCellWidth, "✗")
			} else {
				rowStr += fmt.Sprintf("%*d", adcCellWidth, live.ADC)
			}
		}
		lines = append(lines, rowStr)
	}
	return strings.Join(lines, "\n")
}

func renderTravelTable(s *ECState, cols int) string {
	line0 := tableHeaderStyle.Render("[Travel %]")
	line1 := strings.Repeat(" ", rowLabelWidth) + colHeaders(cols, travelCellWidth)
	lines := make([]string, 2, s.Config.Rows+2)
	lines[0] = line0
	lines[1] = line1

	for r := 0; r < s.Config.Rows; r++ {
		rowStr := fmt.Sprintf("  R%d  ", r)
		for c := 0; c < cols; c++ {
			live := s.Live[r][c]
			cal := s.Cal[r][c]
			keyCalibrated := s.KeyCalibrated(r, c)
			if !live.valid || (!keyCalibrated && cal.Idle == 0) {
				rowStr += fmt.Sprintf("%*s", travelCellWidth, "✗")
			} else {
				t := live.Travel
				cell := fmt.Sprintf("%*d%%", travelCellWidth-1, int(t+0.5))
				if t >= float32(s.Config.ActuationOffset) {
					rowStr += lipgloss.NewStyle().Foreground(lipgloss.Color("#FF2222")).Render(cell)
				} else {
					rowStr += faintStyle.Render(cell)
				}
			}
		}
		lines = append(lines, rowStr)
	}
	return strings.Join(lines, "\n")
}


func waitForLine(ch chan string) tea.Cmd {
	return func() tea.Msg {
		line, ok := <-ch
		if !ok {
			return tea.QuitMsg{}
		}
		return lineMsg(line)
	}
}

func NewModel(lines chan string) Model {
	return Model{
		state: NewECState(0, 0),
		lines: lines,
	}
}

func Run(lines chan string) error {
	p := tea.NewProgram(NewModel(lines), tea.WithAltScreen())
	_, err := p.Run()
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		return err
	}
	return nil
}
