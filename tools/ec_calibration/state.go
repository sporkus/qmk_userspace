package main

import (
	"fmt"
	"math"
)

// Config holds the parsed EC_CFG key=value line.
type Config struct {
	Rows            int
	Cols            int
	ActuationOffset int
	ReleaseOffset   int
	Configured      bool
	BottomingCal    bool
	MinTravel       int
	DefaultBottom   int
	Gamma           float64 // TRAVEL_CURVE_GAMMA from firmware; 1.0 = linear
}

// KeyCal holds per-key calibration data (idle + bottom ADC).
type KeyCal struct {
	Idle   uint16
	Bottom uint16
}

// KeyLive holds per-key live ADC reading and computed travel%.
type KeyLive struct {
	ADC    uint16
	Travel float32 // 0-100%
	valid  bool
}

// ECState holds the full calibration state.
type ECState struct {
	Config  Config
	Cal     [][]KeyCal
	Live    [][]KeyLive
	TuiMode  bool // true when EC_TUI keycode has been pressed on the keyboard
	CalMode  bool // true when bottoming calibration is active
	CalPhase int  // 1 = baseline tuning (hands off), 2 = bottoming phase (press all keys)
	Ready    bool // true after EC_CFG received
}

// NewECState initialises the calibration grids.
func NewECState(rows, cols int) ECState {
	cal := make([][]KeyCal, rows)
	live := make([][]KeyLive, rows)
	for r := 0; r < rows; r++ {
		cal[r] = make([]KeyCal, cols)
		live[r] = make([]KeyLive, cols)
	}
	return ECState{
		Cal:  cal,
		Live: live,
	}
}

// KeyCalibrated reports whether a key's travel exceeds the minimum threshold.
func (s *ECState) KeyCalibrated(row, col int) bool {
	if row < 0 || row >= s.Config.Rows || col < 0 || col >= s.Config.Cols {
		return false
	}
	c := s.Cal[row][col]
	if c.Bottom == 0 && c.Idle == 0 {
		return false
	}
	travel := int(c.Bottom) - int(c.Idle)
	minT := (int(s.Config.DefaultBottom) - int(c.Idle)) * s.Config.MinTravel / 100
	return travel >= minT && travel > 0
}

// SetCal fills idle and bottom values from a parsed CSV row.
func (s *ECState) SetCal(row int, kind string, values []uint16) {
	if row < 0 || row >= s.Config.Rows {
		return
	}
	for c, v := range values {
		if c >= s.Config.Cols {
			break
		}
		switch kind {
		case "idle":
			s.Cal[row][c].Idle = v
		case "bottom":
			s.Cal[row][c].Bottom = v
		}
	}
}

// SetLive fills live ADC values and recomputes travel%.
func (s *ECState) SetLive(row int, values []uint16) {
	if row < 0 || row >= s.Config.Rows {
		return
	}
	gamma := s.Config.Gamma
	if gamma <= 0 {
		gamma = 1
	}
	for c, adc := range values {
		if c >= s.Config.Cols {
			break
		}
		s.Live[row][c].ADC = adc
		s.Live[row][c].Travel = travelPercent(s.Cal[row][c], adc, gamma)
		s.Live[row][c].valid = true
	}
}

// travelPercent computes the gamma-corrected travel percentage.
// raw = (adc - idle) / (bottom - idle); display = raw^gamma * 100
// With gamma > 1 this linearises EC's nonlinear capacitance response.
func travelPercent(cal KeyCal, adc uint16, gamma float64) float32 {
	idle := float64(cal.Idle)
	bottom := float64(cal.Bottom)
	if bottom <= idle {
		return 0
	}
	raw := (float64(adc) - idle) / (bottom - idle)
	if raw < 0 {
		raw = 0
	}
	if raw > 1 {
		raw = 1
	}
	if gamma <= 0 {
		gamma = 1
	}
	return float32(math.Pow(raw, gamma) * 100.0)
}

func (s ECState) String() string {
	out := fmt.Sprintf("ECState{rows=%d cols=%d cal_mode=%v ready=%v}\n",
		s.Config.Rows, s.Config.Cols, s.CalMode, s.Ready)
	out += fmt.Sprintf("  Config: act=%d rel=%d min_travel=%d default_bottom=%d\n",
		s.Config.ActuationOffset, s.Config.ReleaseOffset,
		s.Config.MinTravel, s.Config.DefaultBottom)
	return out
}
