import { createApp } from 'vue'
import { createPinia } from 'pinia'
import App from './App.vue'
import router from './router'
import Vue3Toastify, { type ToastContainerOptions } from 'vue3-toastify'
// import './style.css'
import './styles/main.css'
import 'vue3-toastify/dist/index.css'

const app = createApp(App)
const pinia = createPinia()

app.use(Vue3Toastify, {
	autoClose: 3000,
	position: 'top-right',
	toastClassName: "custom-toast",
	// możesz dostosować więcej opcji według potrzeb
} as ToastContainerOptions)

app.use(router)
app.use(pinia)
app.mount('#app')