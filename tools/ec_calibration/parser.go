package main

import (
	"strconv"
	"strings"
)

// ParseLine strips the QMK console prefix and dispatches on the EC_ tag.
// It mutates s as needed.
func ParseLine(line string, s *ECState) {
	// Strip QMK console prefix: "keyboard:keymap:layer: " → find last ": " and skip
	if idx := strings.LastIndex(line, ": "); idx != -1 {
		line = line[idx+2:]
	}

	if !strings.HasPrefix(line, "EC_") {
		return
	}

	tag, body, ok := strings.Cut(line, ":")
	if !ok {
		return
	}

	switch tag {
	case "EC_CFG":
		parseConfig(body, s)
	case "EC_IDLE":
		parseCalRow(body, s, "idle")
	case "EC_BOTTOM":
		parseCalRow(body, s, "bottom")
	case "EC_ADC":
		parseADCRow(body, s)
	case "EC_KEY_BOTTOM":
		parseKeyBottom(body, s)
	case "EC_EVENT":
		parseEvent(body, s)
	}
}

// parseConfig parses "rows=N,cols=N,act=N,rel=N,min_travel=N,default_bottom=N,configured=N,bottoming_cal=N"
func parseConfig(body string, s *ECState) {
	var rows, cols, act, rel, minTravel, defaultBottom, configured, bCal, gammaScaled int
	for _, kv := range strings.Split(body, ",") {
		parts := strings.SplitN(kv, "=", 2)
		if len(parts) != 2 {
			continue
		}
		key, val := parts[0], parts[1]
		v, _ := strconv.Atoi(val)
		switch key {
		case "rows":
			rows = v
		case "cols":
			cols = v
		case "act":
			act = v
		case "rel":
			rel = v
		case "min_travel":
			minTravel = v
		case "default_bottom":
			defaultBottom = v
		case "configured":
			configured = v
		case "bottoming_cal":
			bCal = v
		case "gamma":
			gammaScaled = v
		}
	}

	if rows <= 0 || cols <= 0 {
		return
	}

	// If rows/cols haven't been set yet (or changed), reinit
	if !s.Ready || s.Config.Rows != rows || s.Config.Cols != cols {
		ns := NewECState(rows, cols)
		ns.Config = s.Config
		ns.TuiMode = s.TuiMode
		ns.CalMode = s.CalMode
		*s = ns
		s.Ready = true
	}

	s.Config.Rows = rows
	s.Config.Cols = cols
	s.Config.ActuationOffset = act
	s.Config.ReleaseOffset = rel
	s.Config.MinTravel = minTravel
	s.Config.DefaultBottom = defaultBottom
	s.Config.Configured = configured != 0
	s.Config.BottomingCal = bCal != 0
	if gammaScaled > 0 {
		s.Config.Gamma = float64(gammaScaled) / 100.0
	}
}

// parseCalRow parses "row:val,val,val,..." and fills idle or bottom values.
func parseCalRow(body string, s *ECState, kind string) {
	colonIdx := strings.Index(body, ":")
	if colonIdx == -1 {
		return
	}
	row, err := strconv.Atoi(body[:colonIdx])
	if err != nil {
		return
	}
	vals := parseCSV(body[colonIdx+1:])
	s.SetCal(row, kind, vals)
}

// parseADCRow parses "row:val,val,val,..." and fills live ADC + travel%.
// Drops the line silently if EC_CFG hasn't arrived yet — the firmware will
// re-emit the config periodically, so the TUI catches up automatically.
func parseADCRow(body string, s *ECState) {
	if !s.Ready {
		return
	}
	colonIdx := strings.Index(body, ":")
	if colonIdx == -1 {
		return
	}
	row, err := strconv.Atoi(body[:colonIdx])
	if err != nil || row < 0 || row >= s.Config.Rows {
		return
	}
	s.SetLive(row, parseCSV(body[colonIdx+1:]))
}

// parseKeyBottom handles "row,col:value" — a per-key bottoming max update during calibration.
func parseKeyBottom(body string, s *ECState) {
	colonIdx := strings.Index(body, ":")
	if colonIdx == -1 {
		return
	}
	coords := strings.SplitN(body[:colonIdx], ",", 2)
	if len(coords) != 2 {
		return
	}
	row, err1 := strconv.Atoi(coords[0])
	col, err2 := strconv.Atoi(coords[1])
	val, err3 := strconv.ParseUint(strings.TrimSpace(body[colonIdx+1:]), 10, 16)
	if err1 != nil || err2 != nil || err3 != nil {
		return
	}
	if row < 0 || row >= s.Config.Rows || col < 0 || col >= s.Config.Cols {
		return
	}
	s.Cal[row][col].Bottom = uint16(val)
	s.Live[row][col].Travel = travelPercent(s.Cal[row][col], s.Live[row][col].ADC, s.Config.Gamma)
}

// parseEvent handles EC_EVENT payloads.
func parseEvent(body string, s *ECState) {
	switch body {
	case "tuiStarted":
		s.TuiMode = true
	case "tuiStopped":
		s.TuiMode = false
		s.CalMode = false
		s.CalPhase = 0
	case "calStarted":
		s.CalMode = true
		s.CalPhase = 1
	case "calTuningDone":
		s.CalPhase = 2
	case "calSaved":
		s.CalMode = false
		s.CalPhase = 0
	}
}

// parseCSV splits a comma-separated string into uint16 values, silently skipping bad tokens.
func parseCSV(s string) []uint16 {
	parts := strings.Split(s, ",")
	vals := make([]uint16, 0, len(parts))
	for _, p := range parts {
		p = strings.TrimSpace(p)
		if p == "" {
			continue
		}
		v, err := strconv.ParseUint(p, 10, 16)
		if err != nil {
			continue
		}
		vals = append(vals, uint16(v))
	}
	return vals
}
