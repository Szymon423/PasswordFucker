<template>
	<div class="login-container">
		<div class="login-box">
			<div class="tabs">
				<button :class="['tab-btn', { active: activeTab === 'login' }]" @click="activeTab = 'login'">
					Login
				</button>
				<button :class="['tab-btn', { active: activeTab === 'register' }]" @click="activeTab = 'register'">
					Register
				</button>
			</div>

			<!-- Formularz logowania -->
			<form v-if="activeTab === 'login'" @submit.prevent="handleLogin">
				<div class="form-group">
					<label for="login">Login</label>
					<input type="text" id="login" v-model="loginForm.login" class="form-input" required />
				</div>
				<div class="form-group">
					<label for="password">Password</label>
					<input type="password" id="password" v-model="loginForm.password" class="form-input" required />
				</div>
				<button type="submit" class="btn btn-login" :disabled="authStore.loading">
					Log in
				</button>
			</form>

			<!-- Formularz rejestracji -->
			<form v-else @submit.prevent="handleRegister">
				<div class="form-group">
					<label for="reg-login">Login</label>
					<input type="text" id="reg-login" v-model="registerForm.login" class="form-input" required />
				</div>
				<div class="form-group">
					<label for="reg-password">Hasło</label>
					<input type="password" id="reg-password" v-model="registerForm.password" class="form-input"
						required />
				</div>
				<div class="form-group">
					<label for="name">Imię</label>
					<input type="text" id="name" v-model="registerForm.name" class="form-input" required />
				</div>
				<div class="form-group">
					<label for="surname">Nazwisko</label>
					<input type="text" id="surname" v-model="registerForm.surname" class="form-input" required />
				</div>
				<button type="submit" class="btn btn-login" :disabled="authStore.loading">
					Register
				</button>
			</form>
		</div>
	</div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '@/stores/auth'
import { toast } from 'vue3-toastify'

const router = useRouter()
const authStore = useAuthStore()
const activeTab = ref('login')

const loginForm = ref({
	login: '',
	password: ''
})

const registerForm = ref({
	login: '',
	password: '',
	name: '',
	surname: ''
})

const handleLogin = async () => {
	const success = await authStore.login(loginForm.value)
	if (success) {
		router.push('/dashboard')
	}
	else {
		toast.error(authStore.error)
	}
}

const handleRegister = async () => {
	const success = await authStore.register({
		...registerForm.value,
		id: 0 // ID zostanie nadane przez backend
	})
	if (success) {
		activeTab.value = 'login'
		registerForm.value = {
			login: '',
			password: '',
			name: '',
			surname: ''
		}
	}
}
</script>

<style scoped>
.login-container {
	display: flex;
	justify-content: center;
	align-items: center;
	min-height: 100vh;
	background-color: var(--bg-primary);
}

.login-box {
	background: var(--bg-secondary);
	padding: 2rem;
	border-radius: 8px;
	border: 1px solid var(--border-color);
	width: 100%;
	max-width: 400px;
}

.app-title {
	text-align: center;
	color: var(--text-primary);
	margin-bottom: 1.5rem;
	font-size: 1.5rem;
}

.tabs {
	display: flex;
	margin-bottom: 20px;
	border-bottom: 1px solid var(--border-color);
}

.tab-btn {
	flex: 1;
	padding: 10px;
	border: none;
	background: none;
	cursor: pointer;
	font-size: 16px;
	color: var(--text-secondary);
	transition: all 0.2s ease;
}

.tab-btn:hover {
	color: var(--text-primary);
}

.tab-btn.active {
	color: var(--text-active);
	border-bottom: 2px solid var(--accent-primary);
}

.form-group {
	margin-bottom: 1rem;
}

label {
	display: block;
	margin-bottom: 0.5rem;
	color: var(--text-primary);
}

.btn-login {
	background-color: var(--bg-secondary);
	border: 2px solid var(--button-green);
	color: white;
	cursor: pointer;
}

.btn-login:hover {
	background-color: var(--button-green-hover);
}

.btn-login:disabled {
	background-color: var(--button-disabled);
	cursor: not-allowed;
}

.error-message {
	color: var(--text-error);
	margin-bottom: 1rem;
	padding: 0.5rem;
	background-color: var(--bg-error);
	border-radius: 4px;
	text-align: center;
}
</style>
