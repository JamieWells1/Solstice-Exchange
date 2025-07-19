#!/usr/bin/env zsh

GREEN="\033[0;32m"
YELLOW="\033[1;33m"
RESET="\033[0m"

SCRIPT="goldpkg/gold"
INSTALL_DIR="$HOME/.local/bin"
SHELL_CONFIG="$HOME/.zshrc"

mkdir -p "$INSTALL_DIR"
chmod +x "$SCRIPT"
cp "$SCRIPT" "$INSTALL_DIR/gold"

if ! grep -q 'export PATH="$HOME/.local/bin:$PATH"' "$SHELL_CONFIG"; then
  echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$SHELL_CONFIG"
fi

echo "\n${GREEN}✓ gold installed to ~/.local/bin${RESET}\n"

if [ -n "$ZSH_VERSION" ]; then
  source "$SHELL_CONFIG"
else
  echo "\n${YELLOW}You're not in a zsh shell. To use 'gold' now, run: source ~/.zshrc${RESET}\n"
fi
