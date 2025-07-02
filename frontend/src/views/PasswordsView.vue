<template>
	<div class="dashboard">
		<div class="dashboard-header">
			<h1>Passwords</h1>
			<button class="btn btn-add" @click="openAddPassword">
				<PlusCircle class="icon" />
				Add password
			</button>
		</div>

		<div class="dashboard-content">
			<div class="search-bar">
				<div class="search-wrapper">
					<Search class="search-icon" />
					<input type="text" v-model="searchQuery" placeholder="Search password..." class="search-input" />
				</div>
			</div>

			<div class="table-container">
				<table v-if="passwords.length > 0">
					<thead>
						<tr>
							<th class="col-name">Name</th>
							<th class="col-login">Login</th>
							<th class="col-url">URL</th>
							<th class="col-actions">Actions</th>
						</tr>
					</thead>
					<tbody>
						<tr v-for="password in filteredPasswords" :key="password.id">
							<td>{{ password.name }}</td>
							<td>
								<div class="password-field">
									<span>{{ password.login }}</span>
									<button class="icon-button" @click="copyToClipboard(password.login)">
										<Copy class="icon" />
									</button>
								</div>
							</td>
							<td>
								<p v-if="password.url" class="url-link">
									{{ password.url }}
								</p>
								<span v-else>-</span>
							</td>
							<td>
								<div class="action-buttons">
									<button class="icon-button preview" @click="showPasswordDetails(password)">
										<Eye class="icon" />
									</button>
									<button class="icon-button copy" @click="copyToClipboard(password.password)">
										<Key class="icon" />
									</button>
									<button class="icon-button edit" @click="openEditPassword(password)">
										<Edit class="icon" />
									</button>
									<button class="icon-button delete" @click="confirmDelete(password)">
										<Trash class="icon" />
									</button>
								</div>
							</td>
						</tr>
					</tbody>
				</table>
				<div v-else class="no-passwords">
					<p>No passwords found</p>
				</div>
			</div>
		</div>

		<div v-if="showAddEditPasswordModal" class="modal">
			<div class="modal-content">
				<div class="modal-header">
					<h2 v-if="addEditMode === 'add'">Add password</h2>
					<h2 v-if="addEditMode === 'edit'">Edit password</h2>
				</div>

				<div class="modal-body">
					<div class="form-group">
						<label for="name">Name:</label>
						<input type="text" id="name" v-model="mutablePassword.name" class="form-input" />
					</div>

					<div class="form-group">
						<label for="login">Login:</label>
						<input type="text" id="login" v-model="mutablePassword.login" class="form-input" />
					</div>

					<div class="form-group">
						<label for="password">Password:</label>
						<div class="password-input-wrapper">
							<input :type="passwordVisible ? 'text' : 'password'" id="password"
								v-model="mutablePassword.password" class="form-input" />

							<button type="button" class="icon-button" @click="passwordVisible = !passwordVisible">
								<Eye v-if="!passwordVisible" class="icon" />
								<EyeOff v-else class="icon" />
							</button>

							<button type="button" class="icon-button generate-password" @click="generatePassword"
								title="Generate password">
								<Wand2 class="icon" />
							</button>

							<button type="button" class="icon-button"
								@click="passwordOptionsVisible = !passwordOptionsVisible" title="Show password options">
								<ChevronDown v-if="!passwordOptionsVisible" class="icon" />
								<ChevronUp v-else class="icon" />
							</button>
						</div>
					</div>

					<div v-if="passwordOptionsVisible" class="password-options-container">
						<!-- Długość hasła -->
						<div class="option-row">
							<label for="password-length">Password length:</label>
							<div class="two-controls" id="password-length">
								<input type="range" v-model.number="mutablePassword.options.minimalLength" min="4" max="64"
									class="slider" />
								<span class="counter">{{ mutablePassword.options.minimalLength }}</span>
							</div>
						</div>

						<!-- Wielkie litery -->
						<div class="option-row">
							<label>Uppercase letters:</label>
							<div class="controls">
								<input type="checkbox" v-model="mutablePassword.options.includeUppercase" />
								<input type="range" v-model.number="mutablePassword.options.uppercaseMinimalNumber" min="0" max="64"
									:disabled="!mutablePassword.options.includeUppercase" class="slider" />
								<span class="counter">{{ mutablePassword.options.uppercaseMinimalNumber }}</span>
							</div>
						</div>

						<!-- Małe litery -->
						<div class="option-row">
							<label>Lowercase letters:</label>
							<div class="controls">
								<input type="checkbox" v-model="mutablePassword.options.includeLowercase" />
								<input type="range" v-model.number="mutablePassword.options.lowercaseMinimalNumber" min="0" max="64"
									:disabled="!mutablePassword.options.includeLowercase" class="slider" />
								<span class="counter">{{ mutablePassword.options.lowercaseMinimalNumber }}</span>
							</div>
						</div>

						<!-- Cyfry -->
						<div class="option-row">
							<label>Digits:</label>
							<div class="controls">
								<input type="checkbox" v-model="mutablePassword.options.includeDigits" />
								<input type="range" v-model.number="mutablePassword.options.digitsMinimalNumber" min="0" max="64"
									:disabled="!mutablePassword.options.includeDigits" class="slider" />
								<span class="counter">{{ mutablePassword.options.digitsMinimalNumber }}</span>
							</div>
						</div>

						<!-- Znaki specjalne -->
						<div class="option-row">
							<label>Special characters:</label>
							<div class="controls">
								<input type="checkbox" v-model="mutablePassword.options.includeSpecialCharacters" />
								<input type="range" v-model.number="mutablePassword.options.specialCharactersMinimalNumber" min="0"
									max="64" :disabled="!mutablePassword.options.includeSpecialCharacters" class="slider" />
								<span class="counter">{{ mutablePassword.options.specialCharactersMinimalNumber }}</span>
							</div>
						</div>

						<!-- Znaki zabronione -->
						<div class="form-group">
							<label for="forbidden-chars">Forbidden characters:</label>
							<input type="text" id="forbidden-chars" v-model="mutablePassword.options.forbiddenCharacters"
								class="form-input" />
						</div>
					</div>

					<div class="form-group">
						<label for="url">URL:</label>
						<input type="text" id="url" v-model="mutablePassword.url" class="form-input" />
					</div>

					<div class="form-group">
						<label for="notes">Notes:</label>
						<input type="text" id="notes" v-model="mutablePassword.notes" class="form-input" />
					</div>
				</div>

				<div class="modal-footer">
					<div class="modal-buttons">
						<button v-if="addEditMode === 'add'" class="btn btn-add" @click="addPassword">
							<Save class="icon" />
							Add
						</button>
						<button v-if="addEditMode === 'edit'" class="btn btn-edit" @click="editPassword">
							<Save class="icon" />
							Edit
						</button>
						<button type="button" @click="showAddEditPasswordModal = false" class="btn btn-cancel">
							<X class="icon" />
							Cancel
						</button>
					</div>
				</div>
			</div>
		</div>

		<!-- Modal do potwierdzenia usunięcia -->
		<div v-if="showDeleteModal" class="modal">
			<div class="modal-content">
				<h2>Confirm deletion</h2>
				<p>Are you sure you want to delete this password?</p>
				<div class="modal-buttons">
					<button @click="deletePassword" class="btn btn-delete">
						<Trash class="icon" />
						Delete
					</button>
					<button @click="showDeleteModal = false" class="btn btn-cancel">
						<X class="icon" />
						Cancel
					</button>
				</div>
			</div>
		</div>
	</div>
</template>

<script setup lang="ts">
import { ref, onMounted, computed } from 'vue';
import { type Password, type Options, passwordsService } from '@/services/passwords.service';
import {
	PlusCircle,
	Search,
	Copy,
	Eye,
	EyeOff,
	Key,
	Edit,
	Trash,
	Save,
	X,
	Wand2,
	ChevronDown,
	ChevronUp
} from 'lucide-vue-next';
import { toast } from 'vue3-toastify'

const passwords = ref<Password[]>([]);
const searchQuery = ref('');
const showDeleteModal = ref(false);
const selectedPassword = ref<Password | null>(null);
const mutablePassword = ref<Password>(passwordsService.getDefaultPassword());
const showAddEditPasswordModal = ref(false);
const addEditMode = ref('add');
const passwordVisible = ref(false);
const passwordOptionsVisible = ref(false);

// Pobieranie haseł
const fetchPasswords = async () => {
	try {
		passwords.value = await passwordsService.getAllPasswords();
	} catch (error) {
		toast.error('Error upon passwords downloading')
	}
};

const addPassword = async () => {
	mutablePassword.value.options = convertOptionsToNumbers(mutablePassword.value.options);
	try {
		// validate required fields
		if (mutablePassword.value.login.length === 0) {
			toast.error("Login can not be empty");
		}
		else if (mutablePassword.value.password.length === 0) {
			toast.error("Password can not be empty");
		}
		else if (mutablePassword.value.password.name === 0) {
			toast.error("Name can not be empty");
		}
		else {
			await passwordsService.createPassword(mutablePassword.value);
			showAddEditPasswordModal.value = false;
		}
	} catch (error) {
		toast.error('Error upon password adding')
	} finally {
		fetchPasswords();
	}
};

const editPassword = async () => {
	mutablePassword.value.options = convertOptionsToNumbers(mutablePassword.value.options);
	try {
		// validate required fields
		if (mutablePassword.value.login.length === 0) {
			toast.error("Login can not be empty");
		}
		else if (mutablePassword.value.password.length === 0) {
			toast.error("Password can not be empty");
		}
		else if (mutablePassword.value.password.name === 0) {
			toast.error("Name can not be empty");
		}
		else {
			await passwordsService.updatePassword(mutablePassword.value);
			showAddEditPasswordModal.value = false;
			toast.success('Password updated successfully');
		}
	} catch (error) {
		toast.error('Error upon password updating')
	} finally {
		fetchPasswords();
	}
};

// Usuwanie hasła
const deletePassword = async () => {
	if (selectedPassword.value) {
		try {
			await passwordsService.deletePassword(selectedPassword.value);
			toast.success('Password was deleted successfully');
		} catch (error) {
			toast.error('Error upon password removal')
		} finally {
			fetchPasswords();
			showDeleteModal.value = false;
		}
	}
};

// Generowanie hasła
const generatePassword = async () => {
	mutablePassword.value.options = convertOptionsToNumbers(mutablePassword.value.options);
	try {
		const newPassword = await passwordsService.generatePassword(mutablePassword.value.options);
		mutablePassword.value.password = newPassword;
	} catch (error) {
		toast.error('Error upon password generation')
	}
};

// Filtrowanie haseł
const filteredPasswords = computed(() => {
	const query = searchQuery.value.toLowerCase();
	return passwords.value.filter(password =>
		password.name.toLowerCase().includes(query) ||
		password.login.toLowerCase().includes(query) ||
		password.url.toLowerCase().includes(query)
	);
});

// Kopiowanie do schowka
const copyToClipboard = async (text: string) => {
	try {
		await navigator.clipboard.writeText(text);
		// Tutaj możesz dodać notyfikację o sukcesie
	} catch (err) {
		toast.error('Could not copy')
	}
};

// Potwierdzenie usunięcia
const confirmDelete = (password: Password) => {
	selectedPassword.value = password;
	showDeleteModal.value = true;
};

function openEditPassword(password: Password) {
	mutablePassword.value = { ...password };
	addEditMode.value = 'edit';
	showAddEditPasswordModal.value = true;
}

function openAddPassword() {
	mutablePassword.value = passwordsService.getDefaultPassword();
	addEditMode.value = 'add';
	showAddEditPasswordModal.value = true;
}

function convertOptionsToNumbers(options: Options): Options {
	return {
		minimalLength: typeof options.minimalLength === 'number'
			? options.minimalLength
			: Number(options.minimalLength),

		includeUppercase: options.includeUppercase,
		includeLowercase: options.includeLowercase,
		includeDigits: options.includeDigits,
		includeSpecialCharacters: options.includeSpecialCharacters,

		uppercaseMinimalNumber: typeof options.uppercaseMinimalNumber === 'number'
			? options.uppercaseMinimalNumber
			: Number(options.uppercaseMinimalNumber),

		lowercaseMinimalNumber: typeof options.lowercaseMinimalNumber === 'number'
			? options.lowercaseMinimalNumber
			: Number(options.lowercaseMinimalNumber),

		digitsMinimalNumber: typeof options.digitsMinimalNumber === 'number'
			? options.digitsMinimalNumber
			: Number(options.digitsMinimalNumber),

		specialCharactersMinimalNumber: typeof options.specialCharactersMinimalNumber === 'number'
			? options.specialCharactersMinimalNumber
			: Number(options.specialCharactersMinimalNumber),

		forbiddenCharacters: options.forbiddenCharacters
	};
}

// Inicjalizacja
onMounted(() => {
	fetchPasswords();
});
</script>

<style scoped>
.dashboard {
	padding: 0rem;
}

.dashboard-header {
	display: flex;
	justify-content: space-between;
	align-items: center;
	margin-bottom: 1rem;
}

.dashboard-content {
	background-color: var(--bg-secondary);
	padding: 1rem;
	border-radius: 4px;
}

.search-bar {
	margin-bottom: 1rem;
}

.search-input {
	width: 100%;
	padding: 0.5rem;
	border: 1px solid var(--border-color);
	border-radius: 4px;
	background-color: var(--bg-input);
	color: var(--text-primary);
}

.table-container {
	overflow-x: auto;
}

.col-name,
.col-login,
.col-url {
	width: 30%;
}

.col-actions {
	width: 10%;
}

.password-field {
	display: flex;
	align-items: center;
	gap: 0.5rem;
}

.action-buttons {
	display: flex;
	gap: 0.5rem;
}

.icon-button {
	padding: 0.25rem 0.5rem;
	background: none;
	border: none;
	color: var(--text-primary);
	cursor: pointer;
	border-radius: 4px;
}

.icon-button:hover {
	background-color: var(--bg-primary);
}

.icon-button.preview:hover {
	color: var(--accent-primary);
}

.icon-button.copy:hover {
	color: var(--accent-secondary);
}

.icon-button.edit:hover {
	color: var(--accent-warning);
}

.icon-button.delete:hover {
	color: var(--accent-error);
}

.btn-add {
	background-color: var(--bg-secondary);
	border: 2px solid var(--button-green);
	color: white;
	cursor: pointer;
	width: 15rem;
}

.btn-add:hover {
	background-color: var(--button-green-hover);
}

.btn-add:disabled {
	background-color: var(--button-disabled);
	cursor: not-allowed;
}

.btn-edit {
	background-color: var(--bg-secondary);
	border: 2px solid var(--button-yellow);
	color: white;
	cursor: pointer;
	width: 15rem;
}

.btn-edit:hover {
	background-color: var(--button-yellow-hover);
}

.btn-edit:disabled {
	background-color: var(--button-disabled);
	cursor: not-allowed;
}

.btn-cancel {
	background-color: var(--bg-secondary);
	border: 2px solid var(--button-grey);
	color: white;
	cursor: pointer;
	width: 15rem;
}

.btn-cancel:hover {
	background-color: var(--button-grey-hover);
}

.btn-cancel:disabled {
	background-color: var(--button-disabled);
	cursor: not-allowed;
}

.btn-delete {
	background-color: var(--bg-secondary);
	border: 2px solid var(--button-red);
	color: white;
	cursor: pointer;
	width: 15rem;
}

.btn-delete:hover {
	background-color: var(--button-red-hover);
}

.btn-delete:disabled {
	background-color: var(--button-disabled);
	cursor: not-allowed;
}

.no-passwords {
	text-align: center;
	padding: 2rem;
	color: var(--text-secondary);
}

/* Modal styles */
.modal {
	position: fixed;
	top: 0;
	left: 0;
	width: 100%;
	height: 100%;
	background-color: rgba(0, 0, 0, 0.5);
	display: flex;
	justify-content: center;
	align-items: center;
}

.modal-content {
	background-color: var(--bg-secondary);
	border-radius: 8px;
	max-width: 500px;
	width: 90%;
	max-height: 75vh;
	display: flex;
	flex-direction: column;
	overflow: hidden;
}

.modal-header {
	padding: 1.5rem 2rem;
	border-bottom: 1px solid var(--border-color);
	display: flex;
	justify-content: space-between;
	align-items: center;
	background-color: var(--bg-secondary);
}

.modal-header h2 {
	margin: 0;
	font-size: 1.25rem;
	font-weight: 600;
}

.modal-body {
	padding: 2rem;
	background-color: var(--bg-secondary);
	overflow-y: auto;
	flex: 1;
}

.modal-footer {
	padding: 1rem 1.5rem 2rem 2rem;
	border-top: 1px solid var(--border-color);
	display: flex;
	justify-content: flex-end;
	gap: 1rem;
	background-color: var(--bg-secondary);
}

/* Stylowanie scrollbara dla zawartości */
.modal-body::-webkit-scrollbar {
	width: 8px;
}

.modal-body::-webkit-scrollbar-track {
	background: var(--bg-tertiary);
	border-radius: 4px;
}

.modal-body::-webkit-scrollbar-thumb {
	background: var(--border-color);
	border-radius: 4px;
}

.modal-body::-webkit-scrollbar-thumb:hover {
	background: var(--text-muted);
}

.modal-buttons {
	display: flex;
	justify-content: flex-end;
	gap: 1rem;
	margin-top: 1rem;
	width: 100%;
}

.form-group {
	margin-bottom: 1rem;
}

.form-group label {
	display: block;
	margin-bottom: 0.5rem;
}

.form-group input {
	width: 100%;
	padding: 0.5rem;
	border: 1px solid var(--border-color);
	border-radius: 4px;
}

.password-input-wrapper {
	display: flex;
	align-items: center;
	gap: 0.5rem;
	width: 100%;
}

.password-input-wrapper input {
	flex: 1;
}

.password-input-wrapper .icon-button {
	padding: 0.5rem;
	background-color: var(--bg-primary);
	border: 1px solid var(--border-color);
	border-radius: 4px;
	color: var(--text-primary);
}

.password-input-wrapper .icon-button:hover {
	background-color: var(--bg-hover);
}

.generate-password {
	color: var(--accent-primary);
}

.icon {
	width: 16px;
	height: 16px;
	margin-right: 4px;
}

.url-link {
	text-decoration: none;
}

.search-wrapper {
	position: relative;
	display: flex;
	align-items: center;
}

.search-icon {
	position: absolute;
	left: 8px;
	width: 16px;
	height: 16px;
	color: var(--text-secondary);
}

.search-input {
	padding-left: 32px;
}

/* Przyciski z ikonami */
.icon-button {
	display: flex;
	align-items: center;
	justify-content: center;
}

/* Password options */
.password-options-container {
	display: flex;
	flex-direction: column;
	gap: 0.5rem;
	padding: 1rem;
	border: 1px solid var(--border-color);
	border-radius: 4px;
	background-color: var(--bg-secondary);
	margin: 1rem 0;
}

.option-row {
	display: block;
	align-items: center;
	gap: 1rem;
}

.option-row label {
	margin-bottom: 0.5rem;
}

.controls {
	display: grid;
	grid-template-columns: 1fr 10fr 1fr;
	align-items: center;
	gap: 0.5rem;
}

.two-controls {
	display: grid;
	grid-template-columns: 11fr 1fr;
	align-items: center;
	gap: 0.5rem;
}

.slider {
	width: 100%;
	height: 4px;
	background: var(--border-color);
	border-radius: 2px;
	outline: none;
}

.counter {
	background-color: var(--bg-primary);
	padding: 0.25rem 0.5rem;
	border-radius: 4px;
	font-size: 0.875rem;
	min-width: 2.5rem;
	text-align: center;
}
</style>
