# gold: Minimal C++ Project CLI Tool

`gold` is a lightweight CLI utility to bootstrap, build, and run a C++ project from the command line.

It simplifies project setup, enforces clean structure, and supports fully portable usage across systems.

---

## 🛠 Features

- One-command project setup
- Dynamic CMakeLists substitution
- Clean build directory management
- Executable discovery and execution
- Self-healing CLI installation
- User config tracking (`gold.cfg`)

---

## 🚀 Usage

### 1. Setup

Run the following after cloning:

```bash
./goldpkg/install.sh
```

This will install `gold`.

After installing `gold` successfully, run:

```bash
gold setup
```

This will:

- Prompt you for a project name
- Replace `PROJECT_NAME` in all `CMakeLists.txt` files
- Build the project into `build/bin/<your_project_name>`
- Configure global access via `~/.local/bin/gold`

---

### 2. Run your project

Once setup is complete, just:

```bash
gold run
```

This rebuilds the project and executes the compiled binary.

---

## 📁 Directory Structure

```text
cpp_skeleton/
├── goldpkg/
│   ├── gold        # Main CLI script
│   └── install.sh    # One-time installer
├── build/          # Auto-created build directory
├── src/            # Your C++ sources
├── CMakeLists.txt  # Template with 'PROJECT_NAME'
└── gold.cfg        # Auto-generated config
```

---

## Notes

- All builds are local to `build/`
- All config is stored in `gold.cfg`
- If you ever break it, change `setup_complete` in `gold.cfg` to `false` and rerun `gold setup`
