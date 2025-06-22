import { app, BrowserWindow } from 'electron'
import path from 'path'
import { fileURLToPath } from 'url'

const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

function createWindow() {
    const win = new BrowserWindow({
        width: 1200,
        height: 800,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false,
            webSecurity: false // tylko do developmentu!
        }
    })

    // Otwórz DevTools automatycznie (pomocne przy debugowaniu)
    win.webContents.openDevTools()

    if (process.env.VITE_DEV_SERVER_URL) {
        win.loadURL(process.env.VITE_DEV_SERVER_URL)
    } else {
        // Użyj path.join dla lepszej kompatybilności między systemami
        win.loadFile(path.join(__dirname, '../dist/index.html'))
    }

    // Dodaj obsługę błędów ładowania
    win.webContents.on('did-fail-load', (event, errorCode, errorDescription) => {
        console.error('Failed to load:', errorCode, errorDescription)
    })
}

app.whenReady().then(() => {
    createWindow()

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow()
        }
    })
})

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})

// Dodaj obsługę błędów
process.on('uncaughtException', (error) => {
    console.error('Uncaught Exception:', error)
})
