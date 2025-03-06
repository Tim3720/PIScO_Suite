#!/usr/bin/env bash

# Check if Nix is installed
if ! command -v nix &>/dev/null; then
    echo "Error: Nix is not installed. Please install Nix before running this script."
    exit 1
fi

# Help message function
show_help() {
    echo "Usage: $0 [optional_arguments...]"
    echo ""
    echo "Options:"
    echo "  --help       Show this help message and exit"
    echo "  [optional_arguments...]:"
    echo "    debug      Build debug versions of each module"      
    echo "    release      Build release versions of each module"      
    echo "Default: release"
    echo ""
    echo "Example:"
    echo "  $0 debug"
    exit 0
}

# Check if the first argument is --help
if [[ "$1" == "--help" ]]; then
    show_help
fi

# Find all module directories (assuming each module has a flake.nix file)
MODULES=("ThreadManagerModule" "BackgroundCorrectionModule" "SegmenterParallel")

COMPILE_ARG="${1:-release}"

# Compile modules in order
for module in "${MODULES[@]}"; do
    if [ -d "$module" ] && [ -f "$module/flake.nix" ]; then
        echo -e "\e[34mBuilding module: $module\e[0m"
        cd "$module" || { echo "Failed to enter $module"; exit 1; }

        # Enter Nix shell and compile the module
        nix develop --command bash -c "./compile.sh $COMPILE_ARG && cmake --build build"

        # Return to the root directory
        cd - >/dev/null || exit
    else
        echo "Skipping $module: Directory or flake.nix not found."
    fi
    echo "-----------------------------------------------------"
done
