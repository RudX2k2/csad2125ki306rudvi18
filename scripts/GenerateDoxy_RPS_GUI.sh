#!/bin/bash

# Define variables
PROJECT_NAME="RPS_GUI"
PROJECT_VERSION="1.0"
PROJECT_DIR="RPS_GUI"
INPUT_DIRS="../$PROJECT_DIR"
OUTPUT_DIR="../docs"
DOXYFILE_PATH="../$PROJECT_DIR/Doxyfile"
INDEX_FILE="$OUTPUT_DIR/html/index.html"

# Check for Doxygen
if ! command -v doxygen &>/dev/null; then
    echo "Doxygen is not installed. Please install it and try again."
    exit 1
fi

# Check for Graphviz (optional)
if ! command -v dot &>/dev/null; then
    echo "Graphviz is not installed. Diagrams will not be generated."
    DOT_AVAILABLE=NO
else
    DOT_AVAILABLE=YES
fi

# Generate a Doxyfile if it doesn't exist
if [[ ! -f "$DOXYFILE_PATH" ]]; then
    echo "Generating default Doxyfile..."
    doxygen -g "$DOXYFILE_PATH"
fi

# Configure the Doxyfile
sed -i "s|^PROJECT_NAME.*|PROJECT_NAME           = \"$PROJECT_NAME\"|" "$DOXYFILE_PATH"
sed -i "s|^PROJECT_NUMBER.*|PROJECT_NUMBER        = $PROJECT_VERSION|" "$DOXYFILE_PATH"
sed -i "s|^OUTPUT_DIRECTORY.*|OUTPUT_DIRECTORY       = $OUTPUT_DIR|" "$DOXYFILE_PATH"
sed -i "s|^INPUT.*|INPUT                  = $INPUT_DIRS|" "$DOXYFILE_PATH"
sed -i "s|^RECURSIVE.*|RECURSIVE              = YES|" "$DOXYFILE_PATH"
sed -i "s|^HAVE_DOT.*|HAVE_DOT               = $DOT_AVAILABLE|" "$DOXYFILE_PATH"

# Add Qt-specific settings
if ! grep -q "PREDEFINED.*Q_OBJECT" "$DOXYFILE_PATH"; then
    sed -i "/^#.*PREDEFINED.*$/a \
PREDEFINED             = Q_OBJECT \\\n                           signals=public \\\n                           slots=public\n" "$DOXYFILE_PATH"
fi

sed -i "s|^ENABLE_PREPROCESSING.*|ENABLE_PREPROCESSING   = YES|" "$DOXYFILE_PATH"
sed -i "s|^MACRO_EXPANSION.*|MACRO_EXPANSION        = YES|" "$DOXYFILE_PATH"
sed -i "s|^EXPAND_ONLY_PREDEF.*|EXPAND_ONLY_PREDEF     = YES|" "$DOXYFILE_PATH"

# Generate documentation
echo "Generating Doxygen documentation..."
doxygen "$DOXYFILE_PATH"

# Check if the index file was created
if [[ -f "$INDEX_FILE" ]]; then
    echo "Documentation generated successfully. Opening $INDEX_FILE..."
    xdg-open "$INDEX_FILE" &>/dev/null || open "$INDEX_FILE" &>/dev/null || echo "Cannot open $INDEX_FILE. Please open it manually."
else
    echo "Documentation generation failed. Check the Doxyfile for errors."
fi

