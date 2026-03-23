melhore essa parte dos developers

👨‍💻 Developers

faça igual a esse aqui 

melhore essa parte dos developers

👨‍💻 Developers

faça igual a esse aqui 


<h1 align="center">Byte in Space 2 🐶🚀💫</h1>

<p align="center">
  <b>The sequel to Byte in Space. A high-performance Space Invaders-inspired arcade game rewritten in C for ultimate speed and precision.</b>
</p>

<hr>

## Developer 🧑‍💻

<table align="center">
  <tr>
    <td align="center">
      <a href="https://github.com/luizmiguelbarbosa">
        <img src="https://avatars.githubusercontent.com/luizmiguelbarbosa" width="100px;" alt="Luiz Miguel Barbosa"/><br />
        <sub><b>Luiz Miguel Barbosa</b></sub>
      </a>
    </td>
  </tr>
</table>

<hr>

## Description 🌌

**Byte in Space 2** is the evolution of the original Python-based project. Moving from Pygame to **C** and **Raylib**, this sequel offers a much more robust architecture, smoother performance, and advanced features like custom shaders and cross-platform compatibility. It stays true to the classic "Space Invaders" essence while pushing the technical limits of a second-period project.

## Folder Structure 📂

The project follows a modular C structure to keep source code, headers, and assets organized:

```bash
├── assets              
│   ├── fonts           
│   ├── images          
│   │   └── sprites     
│   ├── ost             
│   └── shaders         
│
├── external            
│   ├── raylib_linux
│   ├── raylib_macos
│   └── raylib_windows
│
├── include             
├── src                 
├── CMakeLists.txt      
└── .idea               
```
## Libraries Used 📚
```bash
C Language 
Raylib 5.0
CMake
GLSL 
```
## Project Task Distribution 🌌
<p align="center">
<table align="center">
<tr>
<th>Developer</th>
<th>Tasks</th>
</tr>
<tr>
<td><a href="https://github.com/luizmiguelbarbosa">Luiz Miguel Barbosa</a></td>
<td>Developed the entire game engine in C, including memory management, entity systems, custom shaders, and cross-platform build automation.</td>
</tr>
</table>
</p>

## How to Run 🚀

The project is already pre-compiled for quick access. To play the game, follow these steps:

1. **Clone the repository:**
    ```bash
    git clone https://github.com/luizmiguelbarbosa/byte_in_space_2.git
    ```
2. **Navigate to the executable folder:**
   ```bash
   Open the cmake-build-debug directory.
   ```
3. **Run the game:**
   ```bash
   Execute the byte_in_space_2.exe file.
   ```
## Concepts Used
The transition from Python to C allowed the application of much more rigorous concepts. I moved from high-level abstractions to low-level control, utilizing **Manual Memory Management** and **Pointers** to optimize performance and resource handling.

The use of **Structs** was essential for organizing game data, acting as the foundation for the game's architecture. Additionally, I implemented **Custom Shaders (GLSL)** to enhance visual fidelity, providing effects that go beyond the standard drawing functions.

The project also applied **Linear Algebra** for movement and collision vectors, ensuring high precision in the game's physics loop, achieving a much more responsive feel compared to the first version.

## Challenges and Issues
The biggest challenge was the transition from the "managed" world of Python to the manual complexity of C. Managing memory without a garbage collector required a much more disciplined approach to avoid memory leaks and segmentation faults.

Another significant issue was ensuring cross-platform compatibility. Managing different Raylib binaries for Linux, macOS, and Windows within the same repository required a solid understanding of how CMake links external dependencies. These technical hurdles provided a much steeper, yet more rewarding, learning curve than the first project, proving that good architecture is key to a stable game.

<hr>

<p align="center">
  Developed by Luiz Miguel Barbosa
</p>
