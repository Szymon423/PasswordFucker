<template>
	<div class="app-container">
		<nav class="sidebar" v-if="showNav">
			<div class="nav-header">
				<h1 class="nav-title">Password Fucker</h1>
			</div>
			<router-link to="/dashboard" class="nav-link" active-class="active">
				Dashboard
			</router-link>
			<router-link to="/settings" class="nav-link" active-class="active">
				Settings
			</router-link>
		</nav>
		<main class="main-content">
			<router-view></router-view>
		</main>
	</div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useRoute } from 'vue-router'

const route = useRoute()
const showNav = computed(() => route.name !== 'Login')
</script>

<style scoped>
.app-container {
	display: flex;
	min-height: 100vh;
}

.sidebar {
	width: 250px;
	background-color: var(--bg-secondary);
	border-right: 1px solid var(--border-color);
}

.nav-header {
	padding: 1rem;
	border-bottom: 1px solid var(--border-color);
}

.nav-title {
	margin: 0;
	font-size: 1.2rem;
	color: var(--text-primary);
}

.main-content {
	flex: 1;
	padding: 2rem;
}
</style>
