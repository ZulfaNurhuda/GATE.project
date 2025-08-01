# **🚀・GATE - Ganesha Algorithmic Transpiler Engine**

### **Ever Dreamed of Making Your NOTAL Algorithms Run? ✨**

Welcome to GATE! This awesome project is your magical bridge, transforming your brilliant algorithms written in the Bandung Institute of Technology's Algorithmic Notation (NOTAL) into super-functional, executable Pascal code! Think of it as giving your NOTAL ideas a voice that computers can understand and run. No more just writing algorithms – let's make them _do_ things!

---

### **📋・Table of Contents**

- **✨・<a href="#what-is-gate" style="text-decoration: none;">What is GATE?</a>**
- **🛠️・<a href="#getting-started" style="text-decoration: none;">Getting Started</a>**
- **⚙️・<a href="#build-the-engine" style="text-decoration: none;">Build the Engine</a>**
- **🗺️・<a href="#how-to-use" style="text-decoration: none;">How to Use</a>**
- **💻・<a href="#install-fpc" style="text-decoration: none;">Installing Free Pascal Compiler (FPC)</a>**
- **✨・<a href="#compile-pascal" style="text-decoration: none;">Compiling Your Pascal Code</a>**
- **💖・<a href="#contributing" style="text-decoration: none;">Contributing</a>**
- **📜・<a href="#license" style="text-decoration: none;">License</a>**
- **👋・<a href="#about-me" style="text-decoration: none;">About Me!</a>**

---

### <div id="what-is-gate">**✨・What is GATE?**</div>

GATE (Ganesha Algorithmic Transpiler Engine) is a super cool "translation compiler" designed to take your algorithms, written in the unique Algorithmic Notation of Bandung Institute of Technology (NOTAL), and turn them into ready-to-run Pascal programming language scripts! 🚀 Our main goal is to create a seamless, fun bridge between your awesome algorithmic designs and their practical, executable life in Pascal. Get ready to see your algorithms in action! For a comprehensive understanding of NOTAL syntax and its Pascal equivalents, refer to the [Whitepaper Transpiler NOTAL to Pascal](./docs/README.md).

---

### <div id="getting-started">**🛠️・Getting Started (Let's Get This Party Started! 🎉)**</div>

Ready to bring your NOTAL creations to life? Here's how to get GATE up and running in a flash:

1.  **Clone the magic!** ✨
    
    ```bash
    git clone https://github.com/your-username/GATE.project.git
    cd GATE.project
    git submodule update --init --recursive
    ```

2.  **Install a C++ Toolchain** ⚙️

    To build GATE, you'll need a C++ compiler. You can choose between traditional Makefiles or modern CMake. Below are the setup instructions for different platforms.

    #### **Windows**

    - **For MinGW-w64 (for Makefile or CMake):**

        1. **Download MSYS2:** Visit [https://www.msys2.org/](https://www.msys2.org/) and download the installer.
        2. **Install & Update:** Follow the installer and then run `pacman -Syu` in the MSYS2 terminal, repeating as necessary.
        3. **Install Toolchain:** Run `pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake`. This installs GCC, Make, and CMake.
        4. **Add to PATH:** Add `C:\msys64\mingw64\bin` to your system's PATH for easy access.

    - **For MSVC (for CMake):**

        1. **Install Visual Studio:** Download [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) or another version.
        2. **Select Workload:** During installation, choose the **"Desktop development with C++"** workload. This includes the MSVC compiler, CMake, and NMake.

    #### **Linux**
 
    - **Debian/Ubuntu-based:**
    
       ```bash
       sudo apt update
       sudo apt install build-essential cmake
       ```
 
    - **Fedora/RHEL-based:**
    
      ```bash
      sudo dnf groupinstall "Development Tools"
      sudo dnf install cmake
      ```

    - **Arch Linux:**
    
      ```bash
      sudo pacman -S base-devel cmake
      ```

    #### **macOS**

    ```bash
    xcode-select --install
    brew install cmake
    ```

---

### <div id="build-the-engine">**⚙️・Build the Engine (Choose Your Adventure!)**</div>

GATE supports two build systems: **Makefile** (for simplicity on Unix-like systems) and **CMake** (for flexibility across all platforms).

#### **Option 1: Makefile (The Classic Way 📜)**

Perfect for Linux, macOS, or Windows with MSYS2.

- **On Linux, macOS, or MSYS2 (MinGW-w64):**

  Simply run `make` in the root directory:

  ```bash
  make
  ```

This command compiles the project and places the `gate` executable in the `bin/` directory.

#### **Option 2: CMake (The Modern Way ✨)**

CMake is recommended for its flexibility, especially on Windows with different compilers.

- **A. Unix-like (Linux/macOS):**

  ```bash
  # Create and configure the build directory
  cmake -B build

  # Compile the project
  cmake --build build
  ```

- **B. Windows with MSYS2 (MinGW-w64):**

  ```bash
  # Create and configure using the MSYS Makefiles generator
  cmake -B build -G "MSYS Makefiles"

  # Compile the project
  cmake --build build
  ```

- **C. Windows with Visual Studio (MSVC):**

  Open a **Developer Command Prompt for VS** or ensure `cl.exe` and `cmake.exe` are in your PATH.

  ```bash
  # Create a build directory (CMake auto-detects MSVC)
  cmake -B build

  # Compile the project
  cmake --build build
  ```

After a successful build (with either method), the `gate` executable will be waiting for you in the `bin/` directory! 🎉

---

### <div id="how-to-use">**🗺️・How to Use (Your NOTAL to Pascal Journey! 🚀)**</div>

Once GATE is built, you're just one command away from seeing your NOTAL algorithms transformed into Pascal code that you can compile and run! It's like magic, but with code:

```bash
./bin/transpiler -i <your_notal_file.notal> -o <your_pascal_output.pas>
```

Simply replace `<your_notal_file.notal>` with the path to your NOTAL source file, and `<your_pascal_output.pas>` with the name you want for your shiny new Pascal file. This Pascal file will contain the fully translated, executable version of your algorithm, ready to be compiled and run by any Pascal compiler!

---

### <div id="install-fpc">**💻・Installing Free Pascal Compiler (FPC) (Get Ready to Run! 🏃‍♀️)**</div>

To truly bring your transpiled Pascal code to life, you'll need a Pascal compiler! We recommend using **Free Pascal Compiler (FPC)**, a fantastic, open-source compiler that's super easy to get started with. Here's how to install it on your favorite operating system:

#### **Windows**

1.  **Download the Installer:** Head over to the official Free Pascal website's download page: [https://www.freepascal.org/download.html](https://www.freepascal.org/download.html)

2.  **Find the Windows Installer:** Look for the "Windows" section and download the appropriate installer (usually a `.exe` file). Choose the version that matches your system architecture (32-bit or 64-bit).

3.  **Run the Installer:** Once downloaded, double-click the `.exe` file to start the installation wizard. Follow the on-screen prompts.

> [!IMPORTANT]
> During installation, make sure to select the option to "Add Free Pascal to the system PATH" or similar. This will allow you to run `fpc` commands directly from your command prompt! If you miss this, you might need to add it manually later.

#### **Linux**

For most Linux distributions, you can install FPC using your package manager. Here are some common commands:

- **Debian/Ubuntu-based:**
  
  ```bash
  sudo apt update
  sudo apt install fpc
  ```

- **Fedora/RHEL-based:**
  
  ```bash
  sudo dnf install fpc
  ```

- **Arch Linux:**
  
  ```bash
  sudo pacman -S fpc
  ```

#### **macOS**

On macOS, the easiest way to install FPC is using Homebrew:

1.  **Install Homebrew (if you haven't already):**
    
    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```

2.  **Install FPC:**
    
    ```bash
    brew install fpc
    ```

#### **Verify Installation (for all OS)**

After installation, open a new terminal (Command Prompt/PowerShell on Windows, Terminal on Linux/macOS) and type:

```bash
fpc -i
```

If you see information about the FPC version, congratulations! You're all set to compile your Pascal masterpieces! 🎉

---

### <div id="compile-pascal">**✨・Compiling Your Pascal Code (Time to See it Run! 🚀)**</div>

Once you have your shiny new Pascal file (like `my_program.pas` from the previous step) and Free Pascal Compiler (FPC) installed, it's super easy to turn it into an executable program! Here's how to make your code _zoom_! 💨

```bash
fpc <your_pascal_output.pas>
```

For example, if your transpiled file is named `example.pas`:

```bash
fpc example.pas
```

- **On Windows:** This will magically generate an executable file named `example.exe` in the same directory. ✨

- **On Linux/macOS:** This will create an executable file named `example` (or `example.out` depending on your FPC version/configuration) in the same spot. 🐧🍎

Now, for the grand finale! You can run your Pascal program directly from your terminal! Just type the name of the executable and watch it go! 🤩

- **On Windows:**
  
  ```bash
  .\example.exe
  ```

- **On Linux/macOS:**
  
  ```bash
  ./example
  ```

And voilà! Your NOTAL algorithm, now a Pascal executable, is running and ready to impress! 🎉🥳

---

### <div id="contributing">**💖・Contributing (Join the Fun! 🎉)**</div>

Got cool ideas? Found a pesky bug? Want to add an awesome new feature? We'd absolutely love your help! GATE is a community effort, and every contribution, big or small, makes a difference. Check out our `CONTRIBUTING.md` for all the details on how you can jump in and make GATE even better. Let's build something amazing together!

---

### <div id="license">**📜・License (The Legal Bits, but Still Friendly! 🤝)**</div>

This project is open-source and proudly distributed under the MIT License. This means you're free to explore, use, modify, and share GATE! You can find all the nitty-gritty details in the `LICENSE` file. Happy coding!

---

### <div id="about-me">**👋・About Me!**</div>

**Muhammad Zulfa Fauzan Nurhuda** (18224064)  
Just a regular human being who happens to be studying STI at ITB! 😄 Always excited to learn and build cool stuff! 🚀

<img src="https://i.imgur.com/Zp8msEG.png" alt="Logo ITB" height="90" style="border-radius: 10px">