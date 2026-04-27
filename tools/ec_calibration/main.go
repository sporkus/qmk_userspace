package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"os/exec"
	"sync"
)

func main() {
	lines := make(chan string, 256)

	cmd := exec.Command("qmk", "console")

	stdout, err := cmd.StdoutPipe()
	if err != nil {
		fmt.Fprintf(os.Stderr, "error creating stdout pipe: %v\n", err)
		os.Exit(1)
	}
	stderr, err := cmd.StderrPipe()
	if err != nil {
		fmt.Fprintf(os.Stderr, "error creating stderr pipe: %v\n", err)
		os.Exit(1)
	}

	if err := cmd.Start(); err != nil {
		fmt.Fprintf(os.Stderr, "error starting qmk console: %v\n", err)
		fmt.Fprintf(os.Stderr, "Make sure qmk CLI is installed and your keyboard is connected.\n")
		os.Exit(1)
	}

	var wg sync.WaitGroup
	wg.Add(2)

	// Fan in both stdout and stderr lines into the same channel
	for _, r := range []io.ReadCloser{stdout, stderr} {
		go func(pr io.ReadCloser) {
			defer wg.Done()
			scanner := bufio.NewScanner(pr)
			for scanner.Scan() {
				lines <- scanner.Text()
			}
		}(r)
	}

	go func() {
		wg.Wait()
		cmd.Wait()
		close(lines)
	}()

	if err := Run(lines); err != nil {
		os.Exit(1)
	}

	_ = cmd.Process.Kill()
}
