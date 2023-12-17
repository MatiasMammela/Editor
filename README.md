<p align="center">
  <table>
    <tr>
        <td><h1>Editor</h1></td>
        <td><img src="https://docs.gtk.org/gtk4/gtk-logo.svg" alt="GTK Logo" width="100"></td>
        <td><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/3/3a/Tux_Mono.svg/149px-Tux_Mono.svg.png" alt="Tux Logo" width="100"></td>
    </tr>
  </table>
</p>

## About the Project

This project is a text editor for Linux written in C / C++ . It provides two interfaces: a graphical user interface (GUI) using GTK 4 and a text-based user interface (TUI) using ncurses.

The editor supports basic text editing features, such as opening, editing, and saving files. It also includes syntax highlighting for various programming languages, thanks to the integration with GtkSourceView.

## Dependencies

To install the dependencies, use the following command:

```bash
sudo apt-get install libgtk-4-dev libgtksourceview-5-dev libncurses-dev
```

## Building the Project

To build the project, follow these steps:

1. Clone the repository:

```bash
git clone https://github.com/MatiasMammela/Editor.git
```

2. Navigate to the project directory:

```bash
cd Editor
```

3. Create a new directory for the build files and navigate into it:

```bash
mkdir build
cd build
```

4. Run cmake to generate the Makefile and make to build the project:

```bash
cmake ..
make
```

5. After building the project, you can run the TUI or GUI version of the editor with:

```bash
./editor_tui
or
./editor_gui
```
