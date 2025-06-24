import { createApp } from 'vue'
import { createPinia } from 'pinia'
import App from './App.vue'
import router from './router'
import Vue3Toastify, { type ToastContainerOptions } from 'vue3-toastify'
import './styles/main.css'
import './styles/toast.css'
import 'vue3-toastify/dist/index.css'

const app = createApp(App)
const pinia = createPinia()

app.use(Vue3Toastify, {
	autoClose: 3000,
	position: 'top-right',
	hideProgressBar: false,
    closeOnClick: true,
    pauseOnHover: true,
    draggable: true,
	toastClassName: "custom-toast"
} as ToastContainerOptions)

app.use(router)
app.use(pinia)
app.mount('#app')