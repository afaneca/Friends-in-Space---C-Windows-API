// Gateway.cpp : define o ponto de entrada para o aplicativo do console.
//

#include "cabecalho.h"



int pullInfoDoJogo() {
	HANDLE hMapFile, hMutex, hNovaInfo;
	LPCTSTR pBuf;

	// Abre Mutex
	hMutex = OpenMutex(
		MUTEX_ALL_ACCESS,
		FALSE,
		nomeDoMutex); // NAMED MUTEX

	if (!hMutex) {
		_tprintf(TEXT("Erro a abrir o mutex."));
		getwchar();
		exit(EXIT_FAILURE);
	}

	// Abre Evento
	hNovaInfo = OpenEvent(EVENT_ALL_ACCESS,  // Atributos de Segurança
		TRUE,  // TRUE=manual, FALSE=auto
		nomeDoEventoComunicacao   // nome do Evento
	);
	if (hNovaInfo == NULL) {
		_tprintf(_T("Erro a abrir evento de comunicação com servidor - %d"), GetLastError());
		_gettch();
		exit(EXIT_FAILURE);
	}
	// Espera pela sinalização do evento
	DWORD estado = WaitForSingleObject(
		hNovaInfo,   // handle
		INFINITE // timeout
	);

	if (estado == WAIT_OBJECT_0)
	{
		// evento foi sinalizado
		hMapFile = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,    // acesso de escrita/leitura
			FALSE,                    // não herdar o nome
			nomeDaMemoria);                 // nome do objeto de mapeamento

		if (hMapFile == NULL) {
			_tprintf(TEXT("Erro a abrir mapeamento do ficheiro (%d).\n"), GetLastError());
			return 1;
		}
		j = (jogo *)MapViewOfFile(hMapFile,   // handle
			FILE_MAP_READ, // só dar permissões de leitura, de acordo com o enunciado
			0,
			0,
			sizeof(jogo));

		if (j == NULL)
		{
			_tprintf(TEXT("Não foi possível mapear a view do ficheiro (%d).\n"),
				GetLastError());

			CloseHandle(hMapFile);

			return 1;
		}

		_tprintf(_T("\n A estrutura do jogo foi recebida com sucesso."));
		_tprintf(_T("Nr. de Naves Defensoras : %d"), j->nDefensores);
		

		ResetEvent(hNovaInfo); // Volta a dessinalizar o evento
		ReleaseMutex(hMutex);

		UnmapViewOfFile(j);

		CloseHandle(hMapFile);
	}

	
}

int _tmain()
{
	hDLL = LoadLibraryEx(TEXT("mensagens.dll"), NULL, 0);
	if (hDLL == NULL) {
		_tprintf(_T("Problema a abrir DLL.\n"));
		return true;
	}

	// MÉTODO PARA LER DADOS DA MEMÓRIA PARTILHADA (A PARTIR DA DLL)
	ler = (void(*)()) GetProcAddress(hDLL, "recebe");
	if (ler == NULL) {
		_tprintf(_T("Problema a encontrar função.\n"));

		return true;
	}

	// MÉTODO PARA ENVIAR DADOS PARA A MEMÓRIA PARTILHADA (A PARTIR DA DLL)
	escrever = (void(*) (TCHAR*)) GetProcAddress(hDLL, "escrever");
	if (escrever == NULL) {
		_tprintf(_T("Problema a encontrar função.\n"));

		return true;
	}

	// Teste da partilha da estrutura
	
	while (1) {
		pullInfoDoJogo();
	}
	
	_gettch();

	return 0;
}

