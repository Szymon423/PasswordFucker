import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import path from 'path'
import electron from 'vite-plugin-electron'

export default defineConfig({
	plugins: [
		vue(),
		electron([
		{
			entry: 'electron/main.js',
		}
		]),
	],
	resolve: {
		alias: {
			'@': path.resolve(__dirname, './src')
		}
	}
})
