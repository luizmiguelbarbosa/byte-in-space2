<h1 align="center">Byte in Space 2 🐶🚀💫</h1>
  <b>The sequel to Byte in Space. A high-performance Space Invaders-inspired arcade game rewritten in C for ultimate speed and precision.</b>
</p>
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

## Libraries Used 📚

* **C Language:** The core of the project, chosen for high performance and low-level memory control.
* **Raylib:** A powerful C library used for hardware-accelerated 2D graphics, audio handling, and input management.
* **CMake:** Used as the build system to ensure the project can be compiled across different operating systems.
* **GLSL (OpenGL Shading Language):** Utilized for the custom shaders found in the `assets/shaders` folder, providing modern visual effects.

---

## Technical Evolution: Python vs C 🚀

Transitioning from **Byte in Space 1** (Python/Pygame) to **Byte in Space 2** (C/Raylib) represents a significant leap in technical complexity:

* **Memory Management:** Moving from Python’s garbage collection to manual memory management in C.
* **Performance:** Native execution allows for a much higher number of active entities and smoother frame rates.
* **Modern Graphics:** Implementation of shaders and post-processing, which were not present in the first version.
* **Portability:** The use of `external` libraries for Linux, macOS, and Windows ensures the game runs everywhere natively.

---

## Concepts Applied 🧠

This project serves as a deep dive into systems programming and game development fundamentals:

* **Struct-Based Architecture:** Organization of game data using C structures for Players, Enemies, and Projectiles.
* **The Game Loop:** Implementation of a precise frame-independent update and render cycle.
* **Linear Algebra:** Using vectors for smooth movement, rotations, and collision detection.
* **File I/O:** Loading custom fonts, textures, and shaders directly from the asset directory.

---

## Challenges and Lessons 🛠️

Developing this sequel solo in **C** presented unique challenges compared to the first game:
* **Debugging:** Identifying memory leaks and pointer errors required a much more disciplined approach to coding.
* **Platform Compatibility:** Managing different binaries for multiple operating systems within the `external` folder.
* **Refactoring:** Learning that a well-defined `include` structure is vital for scaling a C project without creating circular dependencies.
