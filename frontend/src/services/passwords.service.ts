// src/services/passwords.service.ts

import axios from 'axios';
import { useAuthStore } from '@/stores/auth';

// Interfejs reprezentujący model hasła
export interface Password {
	id: number;
	userId: number;
	login: string;
	password: string;
	name: string;
	url: string;
	notes: string;
	createdAt: string;
	updatedAt: string;
}

class PasswordsService {
	private readonly baseUrl: string;

	constructor() {
		this.baseUrl = 'http://localhost:1234/api/passwords';
	}

	private getAuthHeaders() {
		const authStore = useAuthStore();
		return {
			headers: {
				'Authorization': `Bearer ${authStore.token}`
			}
		};
	}

	getDefaultPassword(): Password {
        return {
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
    }

	// Pobieranie wszystkich haseł użytkownika
	async getAllPasswords(): Promise<Password[]> {
		try {
			const response = await axios.get<Password[]>(`${this.baseUrl}/get`, this.getAuthHeaders());
			return response.data;
		} catch (error) {
			this.handleError(error);
			return [];
		}
	}

	// Dodawanie nowego hasła
	async createPassword(passwordData: Password): Promise<void> {
		try {
			await axios.post<Password>(`${this.baseUrl}/add`, passwordData, this.getAuthHeaders());
		} catch (error) {
			this.handleError(error);
		}
	}

	// Aktualizacja hasła
	async updatePassword(passwordData: Password): Promise<void> {
		try {
			await axios.post(`${this.baseUrl}/update`, passwordData, this.getAuthHeaders());
		} catch (error) {
			this.handleError(error);
		}
	}

	// Usuwanie hasła
	async deletePassword(passwordData: Password): Promise<void> {
		try {
			await axios.post(`${this.baseUrl}/delete`, passwordData, this.getAuthHeaders());
		} catch (error) {
			this.handleError(error);
		}
	}

	private handleError(error: any): void {
		if (axios.isAxiosError(error)) {
			if (error.response?.status === 401) {
				const authStore = useAuthStore();
				authStore.logout();
			}

			const errorMessage = error.response?.data?.message || 'Wystąpił błąd podczas operacji na haśle';
			throw new Error(errorMessage);
		}
		throw error;
	}
}

// Eksport instancji serwisu
export const passwordsService = new PasswordsService();
