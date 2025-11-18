#!/usr/bin/env bash
# Sync documentation to GitHub Wiki
# Usage: ./sync_wiki.sh

set -euo pipefail

WIKI_DIR="wiki"
DOCS_DIR="docs"

echo "📚 Syncing documentation to GitHub Wiki..."

# Check if wiki is cloned
if [ ! -d "$WIKI_DIR" ]; then
    echo "Cloning wiki repository..."
    git clone https://github.com/Barbatos6669/PiPinPP.wiki.git "$WIKI_DIR"
fi

cd "$WIKI_DIR"
git pull origin master

# Copy documentation files
echo "Copying documentation files..."

# Home page
cp "../$DOCS_DIR/WIKI_HOME.md" "Home.md"

# Sidebar
cp "../$DOCS_DIR/_Sidebar.md" "_Sidebar.md"

# Main documentation
cp "../$DOCS_DIR/INSTALL.md" "Installation.md"
cp "../$DOCS_DIR/GETTING_STARTED.md" "Getting-Started.md"
cp "../$DOCS_DIR/BUILD.md" "Build-Instructions.md"
cp "../$DOCS_DIR/API_REFERENCE.md" "API-Reference.md"
cp "../$DOCS_DIR/CLI_USAGE.md" "CLI-Tool.md"
cp "../$DOCS_DIR/PIN_NUMBERING.md" "Pin-Numbering.md"
cp "../$DOCS_DIR/PLATFORMS.md" "Platform-Support.md"
cp "../$DOCS_DIR/TROUBLESHOOTING.md" "Troubleshooting.md"
cp "../$DOCS_DIR/FAQ.md" "FAQ.md"
cp "../$DOCS_DIR/DEVELOPER.md" "Developer-Guide.md"
cp "../$DOCS_DIR/CODE_STANDARDS.md" "Code-Standards.md"
cp "../$DOCS_DIR/TESTING.md" "Testing.md"
cp "../$DOCS_DIR/ROADMAP.md" "Roadmap.md"
cp "../$DOCS_DIR/PERMISSIONS.md" "Permissions.md"

# Tutorials
echo "Copying tutorials..."
cp "../$DOCS_DIR/tutorials/LED_BLINK.md" "Tutorial-LED-Blink.md"
cp "../$DOCS_DIR/tutorials/BUTTON_INPUT.md" "Tutorial-Button-Input.md"
cp "../$DOCS_DIR/tutorials/TRAFFIC_LIGHT.md" "Tutorial-Traffic-Light.md"
cp "../$DOCS_DIR/tutorials/PWM_BASICS.md" "Tutorial-PWM-Basics.md"
cp "../$DOCS_DIR/tutorials/INTERRUPTS_101.md" "Tutorial-Interrupts.md"
cp "../$DOCS_DIR/tutorials/I2C_SENSOR.md" "Tutorial-I2C-Sensor.md"
cp "../$DOCS_DIR/tutorials/MULTI_THREADING.md" "Tutorial-Multi-Threading.md"
cp "../$DOCS_DIR/tutorials/README.md" "Tutorials-Index.md"

# Fix internal links to match wiki naming
echo "Fixing internal links..."
find . -name "*.md" -type f -exec sed -i \
    -e 's/](INSTALL\.md)/](Installation)/g' \
    -e 's/](GETTING_STARTED\.md)/](Getting-Started)/g' \
    -e 's/](BUILD\.md)/](Build-Instructions)/g' \
    -e 's/](API_REFERENCE\.md)/](API-Reference)/g' \
    -e 's/](CLI_USAGE\.md)/](CLI-Tool)/g' \
    -e 's/](PIN_NUMBERING\.md)/](Pin-Numbering)/g' \
    -e 's/](PLATFORMS\.md)/](Platform-Support)/g' \
    -e 's/](TROUBLESHOOTING\.md)/](Troubleshooting)/g' \
    -e 's/](FAQ\.md)/](FAQ)/g' \
    -e 's/](DEVELOPER\.md)/](Developer-Guide)/g' \
    -e 's/](CODE_STANDARDS\.md)/](Code-Standards)/g' \
    -e 's/](TESTING\.md)/](Testing)/g' \
    -e 's/](ROADMAP\.md)/](Roadmap)/g' \
    -e 's/](PERMISSIONS\.md)/](Permissions)/g' \
    -e 's/](tutorials\/LED_BLINK\.md)/](Tutorial-LED-Blink)/g' \
    -e 's/](tutorials\/BUTTON_INPUT\.md)/](Tutorial-Button-Input)/g' \
    -e 's/](tutorials\/TRAFFIC_LIGHT\.md)/](Tutorial-Traffic-Light)/g' \
    -e 's/](tutorials\/PWM_BASICS\.md)/](Tutorial-PWM-Basics)/g' \
    -e 's/](tutorials\/INTERRUPTS_101\.md)/](Tutorial-Interrupts)/g' \
    -e 's/](tutorials\/I2C_SENSOR\.md)/](Tutorial-I2C-Sensor)/g' \
    -e 's/](tutorials\/MULTI_THREADING\.md)/](Tutorial-Multi-Threading)/g' \
    -e 's/](tutorials\/README\.md)/](Tutorials-Index)/g' \
    -e 's/](WIKI_HOME\.md)/](Home)/g' \
    {} \;

# Commit and push
echo "Committing changes..."
git add -A

if git diff --staged --quiet; then
    echo "✅ No changes to commit"
else
    git commit -m "docs: sync documentation from main repo ($(date +%Y-%m-%d))"
    git push origin master
    echo "✅ Wiki updated successfully!"
fi

cd ..
echo "🎉 Documentation sync complete!"
