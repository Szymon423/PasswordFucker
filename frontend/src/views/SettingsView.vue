<template>
    <div class="settings">
        <h1>Settings</h1>
        <div class="settings-content">
            <div class="settings-section">
                <h3>User Settings</h3>
                <div class="form-group">
                    <label for="theme">Theme</label>
                    <select id="theme" class="form-input">
                        <option value="dark">Dark</option>
                        <option value="light">Light</option>
                    </select>
                </div>
                <!-- Dodaj więcej opcji ustawień -->
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
// Logika komponentu
</script>

<style scoped>
.settings {
    padding: 1rem;
}

.settings-content {
    background-color: var(--bg-secondary);
    padding: 1rem;
    border-radius: 4px;
}

.settings-section {
    margin-bottom: 2rem;
}

h3 {
    color: var(--text-secondary);
    margin-bottom: 1rem;
}
</style>
