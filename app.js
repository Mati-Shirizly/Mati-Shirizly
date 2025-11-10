// Initialize the application
document.addEventListener('DOMContentLoaded', function() {
    // DOM Elements
    const fileInput = document.getElementById('file-input');
    const dropZone = document.getElementById('drop-zone');
    const previewContainer = document.getElementById('preview-container');
    const markdownPreview = document.getElementById('markdown-preview');
    const themeSelect = document.getElementById('theme');
    const printBtn = document.getElementById('print-btn');
    const pdfBtn = document.getElementById('pdf-btn');

    // Configure marked.js options
    marked.setOptions({
        breaks: true,
        gfm: true,
        headerIds: true,
        mangle: false,
        sanitize: false
    });

    // Theme Management
    function initTheme() {
        const savedTheme = localStorage.getItem('md-viewer-theme') || 'system';
        themeSelect.value = savedTheme;
        applyTheme(savedTheme);
    }

    function applyTheme(theme) {
        if (theme === 'system') {
            const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
            document.documentElement.setAttribute('data-theme', prefersDark ? 'dark' : 'light');
        } else {
            document.documentElement.setAttribute('data-theme', theme);
        }
        localStorage.setItem('md-viewer-theme', theme);
    }

    themeSelect.addEventListener('change', function(e) {
        applyTheme(e.target.value);
    });

    // Listen for system theme changes when system theme is selected
    window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', (e) => {
        if (themeSelect.value === 'system') {
            document.documentElement.setAttribute('data-theme', e.matches ? 'dark' : 'light');
        }
    });

    // File handling
    fileInput.addEventListener('change', function(e) {
        const file = e.target.files[0];
        if (file) {
            handleFile(file);
        }
    });

    // Drag and drop functionality
    dropZone.addEventListener('dragover', function(e) {
        e.preventDefault();
        e.stopPropagation();
        dropZone.classList.add('dragover');
    });

    dropZone.addEventListener('dragleave', function(e) {
        e.preventDefault();
        e.stopPropagation();
        dropZone.classList.remove('dragover');
    });

    dropZone.addEventListener('drop', function(e) {
        e.preventDefault();
        e.stopPropagation();
        dropZone.classList.remove('dragover');

        const files = e.dataTransfer.files;
        if (files.length > 0) {
            const file = files[0];
            if (file.name.endsWith('.md') || file.name.endsWith('.markdown')) {
                handleFile(file);
            } else {
                alert('Please drop a Markdown file (.md or .markdown)');
            }
        }
    });

    // Handle file reading and rendering
    function handleFile(file) {
        const reader = new FileReader();

        reader.onload = function(e) {
            const markdownText = e.target.result;
            renderMarkdown(markdownText);

            // Hide drop zone, show preview
            dropZone.style.display = 'none';
            previewContainer.classList.remove('hidden');

            // Enable action buttons
            printBtn.disabled = false;
            pdfBtn.disabled = false;
        };

        reader.onerror = function() {
            alert('Error reading file. Please try again.');
        };

        reader.readAsText(file);
    }

    // Render markdown to HTML
    function renderMarkdown(markdownText) {
        try {
            const html = marked.parse(markdownText);
            markdownPreview.innerHTML = html;
        } catch (error) {
            console.error('Error parsing markdown:', error);
            alert('Error parsing markdown file. Please check the file format.');
        }
    }

    // Print functionality
    printBtn.addEventListener('click', function() {
        window.print();
    });

    // PDF Export functionality with proper page breaks
    pdfBtn.addEventListener('click', function() {
        // Disable button during generation
        pdfBtn.disabled = true;
        pdfBtn.innerHTML = '<svg width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><circle cx="8" cy="8" r="7" stroke="currentColor" stroke-width="2" fill="none" opacity="0.25"/><path d="M8 1 A7 7 0 0 1 15 8" stroke="currentColor" stroke-width="2" fill="none"><animateTransform attributeName="transform" type="rotate" from="0 8 8" to="360 8 8" dur="1s" repeatCount="indefinite"/></path></svg> Generating PDF...';

        // Clone the content for PDF
        const element = markdownPreview.cloneNode(true);

        // Create a temporary container with proper styling
        const container = document.createElement('div');
        container.style.padding = '20px';
        container.style.backgroundColor = 'white';
        container.style.color = 'black';
        container.appendChild(element);

        // Apply styles to prevent breaks
        const styles = `
            <style>
                * {
                    -webkit-print-color-adjust: exact !important;
                    color-adjust: exact !important;
                    print-color-adjust: exact !important;
                }

                body {
                    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
                    line-height: 1.6;
                    color: #333;
                    background: white;
                }

                h1, h2, h3, h4, h5, h6 {
                    page-break-after: avoid;
                    break-after: avoid;
                    page-break-inside: avoid;
                    break-inside: avoid;
                    margin-top: 1.5em;
                    margin-bottom: 0.5em;
                    font-weight: 600;
                    color: #000;
                }

                h1 {
                    font-size: 2.5em;
                    border-bottom: 2px solid #e0e0e0;
                    padding-bottom: 0.3em;
                }

                h2 {
                    font-size: 2em;
                    border-bottom: 1px solid #e0e0e0;
                    padding-bottom: 0.3em;
                }

                h3 { font-size: 1.5em; }
                h4 { font-size: 1.25em; }
                h5 { font-size: 1.1em; }
                h6 { font-size: 1em; }

                p {
                    margin-bottom: 1em;
                    page-break-inside: avoid;
                    break-inside: avoid;
                    orphans: 3;
                    widows: 3;
                }

                ul, ol {
                    margin-bottom: 1em;
                    padding-left: 2em;
                    page-break-inside: avoid;
                    break-inside: avoid;
                }

                li {
                    margin-bottom: 0.25em;
                }

                pre {
                    background-color: #f4f4f4;
                    border: 1px solid #ddd;
                    border-radius: 4px;
                    padding: 1em;
                    overflow-x: auto;
                    margin-bottom: 1em;
                    page-break-inside: avoid;
                    break-inside: avoid;
                }

                code {
                    background-color: #f4f4f4;
                    padding: 0.2em 0.4em;
                    border-radius: 3px;
                    font-family: 'Monaco', 'Consolas', 'Courier New', monospace;
                    font-size: 0.9em;
                }

                pre code {
                    background-color: transparent;
                    padding: 0;
                }

                blockquote {
                    border-left: 4px solid #2563eb;
                    padding-left: 1em;
                    margin: 1em 0;
                    color: #666;
                    font-style: italic;
                    page-break-inside: avoid;
                    break-inside: avoid;
                }

                img {
                    max-width: 100%;
                    height: auto;
                    display: block;
                    margin: 1em 0;
                    page-break-inside: avoid;
                    break-inside: avoid;
                }

                table {
                    border-collapse: collapse;
                    width: 100%;
                    margin-bottom: 1em;
                    page-break-inside: avoid;
                    break-inside: avoid;
                }

                table th,
                table td {
                    border: 1px solid #ddd;
                    padding: 0.75em;
                    text-align: left;
                }

                table th {
                    background-color: #f5f5f5;
                    font-weight: 600;
                }

                table tr:nth-child(even) {
                    background-color: #fafafa;
                }

                hr {
                    border: none;
                    border-top: 2px solid #e0e0e0;
                    margin: 2em 0;
                    page-break-after: avoid;
                    break-after: avoid;
                }

                a {
                    color: #2563eb;
                    text-decoration: none;
                }

                a:hover {
                    text-decoration: underline;
                }
            </style>
        `;

        // PDF options with optimized settings for page breaks
        const opt = {
            margin: [15, 15, 15, 15],
            filename: 'markdown-document.pdf',
            image: { type: 'jpeg', quality: 0.98 },
            html2canvas: {
                scale: 2,
                useCORS: true,
                letterRendering: true,
                scrollY: 0,
                scrollX: 0
            },
            jsPDF: {
                unit: 'mm',
                format: 'a4',
                orientation: 'portrait',
                compress: true
            },
            pagebreak: {
                mode: ['avoid-all', 'css', 'legacy'],
                before: '.page-break-before',
                after: '.page-break-after',
                avoid: ['h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'img', 'table', 'pre', 'blockquote', 'ul', 'ol', 'p']
            }
        };

        // Generate PDF
        html2pdf()
            .set(opt)
            .from(element)
            .save()
            .then(() => {
                // Re-enable button and restore original text
                pdfBtn.disabled = false;
                pdfBtn.innerHTML = '<svg width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><path d="M8 12L3 7h3V1h4v6h3l-5 5z"/><path d="M14 13v1H2v-1h12z"/></svg> Save as PDF';
            })
            .catch((error) => {
                console.error('Error generating PDF:', error);
                alert('Error generating PDF. Please try again.');
                pdfBtn.disabled = false;
                pdfBtn.innerHTML = '<svg width="16" height="16" viewBox="0 0 16 16" fill="currentColor"><path d="M8 12L3 7h3V1h4v6h3l-5 5z"/><path d="M14 13v1H2v-1h12z"/></svg> Save as PDF';
            });
    });

    // Initialize theme on page load
    initTheme();
});
