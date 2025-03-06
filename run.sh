#!/usr/bin/env bash

# Define the list of modules
MODULES=("SegmenterParallel")  # Replace with actual module names

# Help message function
show_help() {
    echo "Usage: $0 <module_name> [additional_arguments...]"
    echo ""
    echo "Options:"
    echo "  --help       Show this help message and exit"
    echo "  <module_name>  The module to run. Options are: ${MODULES[*]}"
    echo "  [args...]     Additional arguments passed to the module"
    echo ""
    echo "Example:"
    echo "  $0 SegmenterParallel /path/to/input.ini"
    exit 0
}

# Check if the first argument is --help
if [[ "$1" == "--help" ]]; then
    show_help
fi


# Check if a module name is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <module_name> [additional_arguments...]"
    exit 1
fi

MODULE_NAME="$1"
shift  # Remove the first argument (module name) so that $@ contains only additional arguments

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd ) # directory of script

# Check if the module exists
if [[ ! " ${MODULES[@]} " =~ " ${MODULE_NAME} " ]]; then
    echo "Error: Unknown module '$MODULE_NAME'. Available modules: ${MODULES[*]}"
    exit 1
fi

echo "Running module: $MODULE_NAME with arguments: " "$@"

# Change to module directory and execute the module with additional arguments
{
    cd "$SCRIPT_DIR" || exit 1
    cd "$MODULE_NAME" || exit 1
    # Run the module inside Nix shell
    ./run.sh "$@"
}

