<h1 align="center">
  RISC-V Assembly Lab 2 
</h1>

<p align="center">
  A set of RISC-V assembly exercises developed for the Computer Architecture course at UFPE using the CompSim simulator.
</p>

<hr>

## Developers 🧑‍💻

<table align="center">
  <tr>
    <td align="center">
      <a href="https://github.com/luizmiguelbarbosa">
        <img src="https://avatars.githubusercontent.com/luizmiguelbarbosa" width="100px;" alt="Luiz Miguel Barbosa"/><br />
        <sub><b>Luiz Miguel Barbosa</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/gustavocharamba">
        <img src="https://avatars.githubusercontent.com/gustavocharamba" width="100px;" alt="Gustavo Charamba"/><br />
        <sub><b>Gustavo Charamba</b></sub>
      </a>
    </td>
  </tr>
</table>

<hr>

<h2>📖 Description</h2>

<p>
This repository contains a set of <strong>RISC-V assembly programming exercises</strong> developed as part of the 
<strong>Computer Architecture</strong> course at UFPE.
</p>

<p>
All activities were implemented and tested using the <strong>CompSim simulator</strong>, exploring both 
<strong>low-level computation</strong> and <strong>hardware interaction concepts</strong> such as memory-mapped I/O.
</p>

<hr>

<h2>📁 Folder Structure</h2>

<pre>
lab2/
 ├── activity1.asm
 ├── activity2.asm
 ├── activity3.asm
 ├── activity4.asm
 ├── activity5.asm
 └── activity6.asm
</pre>

<hr>

<h2>🧪 Activities</h2>

<h3>Activity 1 — Conditional Assignment</h3>
<p>
Implements a conditional logic where a variable <strong>m</strong> is updated based on the comparison between 
<strong>a</strong> and <strong>b</strong>.
</p>

<p><strong>Focus:</strong></p>
<p>
• Branch instructions<br>
• Register comparison<br>
• Basic control flow
</p>

<hr>

<h3>Activity 2 — If-Else Structure</h3>
<p>
Expands the previous activity by implementing a complete <strong>if-else structure</strong>.
</p>

<p><strong>Behavior:</strong></p>
<p>
• Computes a sum (<strong>a + b</strong>)<br>
• Or computes a difference (<strong>a - b</strong>)
</p>

<p><strong>Focus:</strong></p>
<p>
• Conditional branching<br>
• Structured control flow in assembly
</p>

<hr>

<h3>Activity 3 — Flowchart Implementation</h3>
<p>
Translates a flowchart into RISC-V assembly instructions.
</p>

<p><strong>Logic:</strong></p>
<p>
If <strong>i == j</strong>:<br>
→ f = g + h<br><br>
Else:<br>
→ f = g - h
</p>

<p><strong>Focus:</strong></p>
<p>
• Mapping high-level logic to assembly<br>
• Decision structures using branches
</p>

<hr>

<h3>Activity 4 — String Output (Memory Traversal)</h3>
<p>
Implements a loop to iterate through memory and print a string character by character.
</p>

<p><strong>Focus:</strong></p>
<p>
• Memory addressing<br>
• Loop construction<br>
• Sequential data access
</p>

<hr>

<h3>Activity 5 — Keyboard Input & Loopback</h3>
<p>
Implements a <strong>loopback system</strong> that continuously reads characters from the keyboard and prints them back to the screen.
</p>

<p><strong>Behavior:</strong></p>
<p>
• Terminates when the character <strong>'*'</strong> is entered
</p>

<p><strong>Focus:</strong></p>
<p>
• Memory-mapped input/output<br>
• ASCII value handling<br>
• Conditional termination<br>
• Infinite loop control
</p>

<hr>

<h3>Activity 6 — GPIO Simulation (LED Control)</h3>
<p>
Simulates a hardware control system using memory-mapped GPIO within CompSim.
</p>

<p><strong>System behavior:</strong></p>
<p>
• 6 LEDs mapped to GPIO pins (2 to 7)<br>
• Starts with the first LED turned on<br>
• On button press, the active LED shifts to the next position<br>
• Stops when the last LED is reached
</p>

<p><strong>Focus:</strong></p>
<p>
• Memory-mapped I/O<br>
• Bit shifting operations<br>
• Hardware-level logic simulation<br>
• Event-driven behavior
</p>

<hr>

<h2>🚀 How to Run</h2>

<pre>
1. Open the CompSim simulator

2. Load the desired .asm file

3. Assemble the program

4. Run the simulation

5. Observe:
   - Register values
   - Memory changes
   - Console output
   - I/O behavior (keyboard and GPIO)
</pre>

<hr>

<h2>🧠 Concepts Used</h2>

<p>
• RISC-V instruction set<br>
• Branch instructions (beq, bne, blt, etc.)<br>
• Arithmetic operations (add, sub)<br>
• Register usage conventions<br>
• Loops and control flow<br>
• Memory addressing<br>
• ASCII manipulation<br>
• Memory-mapped I/O<br>
• Bitwise and shift instructions<br>
• GPIO simulation
</p>

<hr>

<h2>⚠ Challenges and Issues</h2>

<p>
One of the main challenges in this lab was translating high-level logic (pseudo-code and flowcharts) into low-level RISC-V instructions.
</p>

<p>
Another difficulty was working with <strong>memory-mapped I/O</strong>, especially handling keyboard input, output synchronization, and simulating hardware behavior such as GPIO control.
</p>

<hr>

<p align="center">
Developed by Luiz Miguel Barbosa & Gustavo Charamba
</p>
