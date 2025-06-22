import { ref } from 'vue'
import { defineStore } from 'pinia'

export const useAuthStore = defineStore('auth', () => {
    const isAuthenticated = ref(false)
    const user = ref(null)

    function login(username: string, password: string) {
        // Tutaj później dodasz prawdziwą logikę logowania
        isAuthenticated.value = true
        user.value = { username }
        return true
    }

    function logout() {
        isAuthenticated.value = false
        user.value = null
    }

    return {
        isAuthenticated,
        user,
        login,
        logout
    }
})
