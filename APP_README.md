# MDview.app - iOS Standalone App

A beautiful, lightweight web-based markdown document reader that works as a standalone iOS app.

## Features

- 📄 Upload and read markdown files with beautiful formatting
- 🎨 Three theme options: Light, Dark, and System
- 📱 iOS standalone app support
- 🖨️ Print documents
- 📥 Export to PDF with smart page breaks
- 🔄 Drag & drop file support
- 💾 Offline capability with service worker
- 🎯 No page breaks in the middle of headlines or paragraphs

## How to Install on iOS

### Step 1: Open in Safari
1. Open Safari on your iPhone or iPad
2. Navigate to the app URL where this app is hosted

### Step 2: Add to Home Screen
1. Tap the **Share** button (square with arrow pointing up) at the bottom of Safari
2. Scroll down and tap **"Add to Home Screen"**
3. You'll see the app icon and name "MDview.app"
4. Tap **"Add"** in the top right

### Step 3: Launch the App
1. Find the "MDview.app" icon on your home screen with a blue gradient icon showing a markdown document
2. Tap to launch - it will open in standalone mode (no Safari UI)
3. The app will now work like a native iOS app!

## Usage

### Reading Documents
1. Tap **"Upload Markdown File"** button
2. Select a `.md` or `.markdown` file from your device
3. The document will be displayed with beautiful formatting

### Changing Themes
- Use the **Theme** dropdown to switch between:
  - **Light**: White background, dark text
  - **Dark**: Dark background, light text
  - **System**: Automatically matches your iOS system preference

### Exporting
- **Print**: Tap the Print button to print the document
- **Export to PDF**: Save the formatted document as a PDF file

### Drag & Drop
- You can also drag and drop markdown files directly onto the app

## Offline Support

The app includes a service worker that caches resources for offline use:
- View previously loaded documents without internet
- All core functionality works offline
- Export and print features remain available

## Technical Details

### PWA Features
- Standalone display mode (no browser chrome)
- Custom app icon
- iOS safe area support (works with notch and home indicator)
- Theme color integration
- Offline capability

### iOS Optimizations
- `viewport-fit=cover` for full-screen display
- Safe area insets for proper spacing
- Apple touch icon support
- Status bar styling
- Standalone mode detection

### Page Break Prevention
Both PDF export and printing prevent awkward page breaks:
- Headings stay with following content
- Paragraphs don't split across pages
- List items remain intact
- Code blocks and blockquotes stay together
- Table rows don't break

## Browser Support

- iOS Safari 11.3+ (for standalone app features)
- Any modern browser for basic functionality
- Best experience on iOS devices

## Files

- `index.html` - Main application file
- `manifest.json` - PWA manifest for app installation
- `sw.js` - Service worker for offline support

## Development

This is a single-page application with no build process required:
- Pure HTML, CSS, and JavaScript
- Uses CDN libraries:
  - marked.js for markdown parsing
  - html2pdf.js for PDF generation
- No dependencies to install
- Just open `index.html` in a browser

## License

Open source - feel free to use and modify!
