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
				<h2 v-if="addEditMode === 'add'">Add password</h2>
				<h2 v-if="addEditMode === 'edit'">Edit password</h2>

				<div class="form-group">
					<label for="name">Name:</label>
					<input type="text" id="name" v-model="mutablePassword.name" class="form-input"/>
				</div>

				<div class="form-group">
					<label for="login">Login:</label>
					<input type="text" id="login" v-model="mutablePassword.login" class="form-input"/>
				</div>

				<div class="form-group">
					<label for="password">Password:</label>
					<input type="password" id="password" v-model="mutablePassword.password" class="form-input"/>
				</div>

				<div class="form-group">
					<label for="url">URL:</label>
					<input type="text" id="url" v-model="mutablePassword.url" class="form-input"/>
				</div>

				<div class="form-group">
					<label for="notes">Notes:</label>
					<input type="text" id="notes" v-model="mutablePassword.notes" class="form-input"/>
				</div>

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
import { Password, passwordsService } from '@/services/passwords.service';
import {
	PlusCircle,
	Search,
	Copy,
	Eye,
	Key,
	Edit,
	Trash,
	Save,
	X
} from 'lucide-vue-next';
import { toast } from 'vue3-toastify'

const passwords = ref<Password[]>([]);
const searchQuery = ref('');
const showDeleteModal = ref(false);
const selectedPassword = ref<Password | null>(null);
const mutablePassword = ref<Password>(passwordsService.getDefaultPassword());
const showAddEditPasswordModal = ref(false);
const addEditMode = ref('add');

// Pobieranie haseł
const fetchPasswords = async () => {
	try {
		passwords.value = await passwordsService.getAllPasswords();
	} catch (error) {
		toast.error('Error upon passwords downloading')
	}
};

const addPassword = async () => {
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
			const success = await passwordsService.deletePassword(selectedPassword.value);
		} catch (error) {
			toast.error('Error upon password removal')
		} finally {
			fetchPasswords();
			showDeleteModal.value = false;
		}	
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
	mutablePassword.value = { 
		id: 0,
		userId: 0,
		login: '',
		password: '',
		name: '',
		url: '',
		notes: '',
		createdAt: '',
		updatedAt: ''
	};
	addEditMode.value = 'add';
	showAddEditPasswordModal.value = true;
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

.col-name, .col-login, .col-url {
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
	padding: 2rem;
	border-radius: 8px;
	max-width: 500px;
	width: 90%;
}

.modal-buttons {
	display: flex;
	justify-content: flex-end;
	gap: 1rem;
	margin-top: 1rem;
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

.modal-buttons {
	display: flex;
	justify-content: flex-end;
	gap: 1rem;
	margin-top: 1rem;
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
    padding-left: 32px; /* Miejsce na ikonę */
}

/* Przyciski z ikonami */
.icon-button {
    display: flex;
    align-items: center;
    justify-content: center;
}
</style>
