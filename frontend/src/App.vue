<template>
	<div class="app-container">
		<nav class="sidebar" v-if="showNav">
			<div class="nav-header">
				<h1 class="nav-title">Password Fucker</h1>
			</div>

			<div class="nav-links">
				<router-link to="/dashboard" class="nav-link" active-class="active">
					<KeyRound class="icon" />
					<span>Passwords</span>
				</router-link>
				<router-link to="/settings" class="nav-link" active-class="active">
					<Settings class="icon" />
					<span>Settings</span>
				</router-link>
			</div>

			<div class="user-section" v-if="authStore.user">
				<div class="user-info">
					<div class="user-avatar">
						<UserCircle class="icon" />
					</div>
					<div class="flex flex-col">
						<span class="user-name">{{ authStore.user.name }} {{ authStore.user.surname }}</span>
					</div>
				</div>
				<button @click="handleLogout" class="logout-btn">
					<LogOut class="icon" />
					<span>Log out</span>
				</button>
			</div>
		</nav>
		<main class="main-content">
			<router-view v-slot="{ Component }">
				<transition name="fade" mode="out-in">
					<component :is="Component" />
				</transition>
			</router-view>
		</main>
	</div>
</template>


<script setup lang="ts">
import { computed } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { useAuthStore } from '@/stores/auth'
import { UserCircle, LogOut, KeyRound, Settings } from 'lucide-vue-next'
import '@/styles/main.css'

const route = useRoute()
const router = useRouter()
const authStore = useAuthStore()
const showNav = computed(() => route.name !== 'Login')

const handleLogout = () => {
	authStore.logout()
	router.push('/login')
}
</script>

<style scoped>
.app-container {
	display: flex;
    height: 100vh;
    width: 100vw;
    overflow: hidden;
}

.sidebar {
	width: 250px;
	background-color: var(--bg-secondary);
	border-right: 1px solid var(--border-color);
	display: flex;
	flex-direction: column;
	flex-shrink: 0;
}

.nav-header {
	padding: var(--spacing-sm);
	border-bottom: 1px solid var(--border-color);
}

.nav-title {
	margin: 0;
	font-size: 1.2rem;
}

.nav-links {
	flex: 1;
	padding: var(--spacing-sm) 0;
}

.nav-link {
	display: flex;
	align-items: center;
	gap: var(--spacing-sm);
	padding: var(--spacing-sm);
	color: var(--text-primary);
	text-decoration: none;
	transition: all 0.2s ease;
}

.nav-link:hover {
	background-color: var(--bg-hover);
}

.nav-link.active {
	background-color: var(--bg-tertiary);
	color: white;
	border-left: 4px solid var(--accent-primary);
}

.nav-link .icon {
	color: white;
}

.nav-link.active .icon {
	color: white;
}

.main-content {
	flex: 1;
	padding: var(--spacing-lg);
	overflow-y: auto;
}

/* User Section */
.user-section {
	padding: var(--spacing-sm);
	border-top: 1px solid var(--border-color);
}

.user-info {
	display: flex;
	align-items: center;
	padding: var(--spacing-xs);
	margin-bottom: var(--spacing-xs);
}

.user-avatar {
	margin-right: var(--spacing-xs);
}

.user-name {
	color: var(--text-primary);
	font-size: 0.9rem;
	font-weight: 500;
}

.logout-btn {
	display: flex;
	align-items: center;
	width: 100%;
	gap: var(--spacing-xs);
	padding: var(--spacing-xs);
	color: var(--accent-error);
	background: transparent;
	border: none;
	border-radius: var(--radius-sm);
	cursor: pointer;
	transition: all 0.2s ease;
}

.logout-btn:hover {
	background-color: var(--bg-hover);
}

/* Transition styles */
.fade-enter-active,
.fade-leave-active {
	transition: opacity 0.2s ease;
}

.fade-enter-from,
.fade-leave-to {
	opacity: 0;
}
</style>
