import { app as o, BrowserWindow as t } from "electron";
import i from "path";
import { fileURLToPath as l } from "url";
const s = l(import.meta.url), c = i.dirname(s);
function n() {
  const e = new t({
    width: 1200,
    height: 800,
    webPreferences: {
      nodeIntegration: !0,
      contextIsolation: !1,
      webSecurity: !1
      // tylko do developmentu!
    }
  });
  e.webContents.openDevTools(), process.env.VITE_DEV_SERVER_URL ? e.loadURL(process.env.VITE_DEV_SERVER_URL) : e.loadFile(i.join(c, "../dist/index.html")), e.webContents.on("did-fail-load", (d, r, a) => {
    console.error("Failed to load:", r, a);
  });
}
o.whenReady().then(() => {
  n(), o.on("activate", () => {
    t.getAllWindows().length === 0 && n();
  });
});
o.on("window-all-closed", () => {
  process.platform !== "darwin" && o.quit();
});
process.on("uncaughtException", (e) => {
  console.error("Uncaught Exception:", e);
});
