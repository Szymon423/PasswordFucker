// stores/auth.ts
import { defineStore } from 'pinia';
import axios from 'axios';
import { jwtDecode } from 'jwt-decode'; 

interface User {
    id: number;
    login: string;
    password: string;
    name: string;
    surname: string;
}

interface LoginPayload {
    login: string;
    password: string;
}

interface TokenPayload {
    id: string;
    name: string;
    surname: string;
    iat: number;
    sub: string;
    app: string;
}

interface AuthState {
    user: User | null;
    token: string | null;
    isAuthenticated: boolean;
    loading: boolean;
    error: string | null;
}

export const useAuthStore = defineStore('auth', {
    state: (): AuthState => ({
        user: null,
        token: localStorage.getItem('token'),
        isAuthenticated: false,
        loading: false,
        error: null,
    }),

    getters: {
        currentUser: (state) => state.user,
    },

    actions: {
        /**
         * Inicjalizacja store'a przy starcie aplikacji
         */
        async init() {
            const token = localStorage.getItem('token');
            if (token) {
                this.setToken(token);
                this.extractUserFromToken(token);
            }
        },

        /**
         * Logowanie użytkownika
         */
        async login(payload: LoginPayload) {
            this.loading = true;
            this.error = null;

            try {
                const response = await axios.post('http://localhost:1234/api/authentication/login', payload);
                const { token, status, message } = response.data;

                if (status === 'success' && token) {
                    this.setToken(token);
                    this.extractUserFromToken(token);
                    this.isAuthenticated = true;
                    return true;
                } else {
                    throw new Error(message || 'Login failed');
                }
            } catch (error: any) {
                this.handleError(error);
                return false;
            } finally {
                this.loading = false;
            }
        },

        /**
         * Rejestracja użytkownika
         */
        async register(user: User) {
            this.loading = true;
            this.error = null;

            try {
                const response = await axios.post('http://localhost:1234/api/authentication/register', user);
                const { status, message } = response.data;

                if (status === 'success') {
                    return true;
                } else {
                    throw new Error(message || 'Registration failed');
                }
            } catch (error: any) {
                this.handleError(error);
                return false;
            } finally {
                this.loading = false;
            }
        },

        /**
         * Wylogowanie użytkownika
         */
        logout() {
            this.clearAuth();
        },

        /**
         * Ustawienie tokena
         */
        setToken(token: string) {
            this.token = token;
            localStorage.setItem('token', token);
            axios.defaults.headers.common['Authorization'] = `Bearer ${token}`;
        },

        /**
         * Ekstrakcja danych użytkownika z tokenu JWT
         */
        extractUserFromToken(token: string) {
            try {
                const decoded = jwtDecode<TokenPayload>(token);
                this.user = {
                    id: parseInt(decoded.id),
                    login: decoded.sub,
                    password: "",
                    name: decoded.name,
                    surname: decoded.surname
                };
            } catch (error) {
                console.error('Error decoding token:', error);
                this.clearAuth();
            }
        },

        /**
         * Wyczyszczenie danych autoryzacji
         */
        clearAuth() {
            this.user = null;
            this.token = null;
            this.isAuthenticated = false;
            localStorage.removeItem('token');
            delete axios.defaults.headers.common['Authorization'];
        },

        /**
         * Obsługa błędów
         */
        handleError(error: any) {
            if (error.response) {
                switch (error.response.status) {
                    case 401:
                        this.error = 'Nieprawidłowy login lub hasło';
                        break;
                    case 400:
                        this.error = 'Nieprawidłowe dane';
                        break;
                    default:
                        this.error = error.response.data.message || 'Wystąpił błąd';
                }
            } else if (error.request) {
                this.error = 'Błąd połączenia z serwerem';
            } else {
                this.error = error.message || 'Wystąpił nieoczekiwany błąd';
            }

            if (error.response?.status === 401) {
                this.clearAuth();
            }
        },
    },
});

// Interceptor dla axios
axios.interceptors.response.use(
    (response) => response,
    async (error) => {
        if (error.response?.status === 401) {
            const authStore = useAuthStore();
            authStore.clearAuth();
        }
        return Promise.reject(error);
    }
);
