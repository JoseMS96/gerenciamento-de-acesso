**Gerenciamento de Acesso**

**Descrição**

Projeto de controle de acesso a duas salas em Linux embarcado (Ubuntu WSL), implementado em C e SQLite. O sistema oferece:

- Menu via interface serial para cadastro e autenticação de usuários (administrador ou comum).
- Gerenciamento de permissões de acesso às portas 1 e 2.
- Registro de eventos (abertura de portas) em banco SQLite.
- Comunicação com controladores de portas via Modbus RTU em serial.
- Webserver HTTP mínimo exibindo painel de eventos em HTML.

**Dependências (Ubuntu WSL)**

Instale as bibliotecas e ferramentas necessárias:

```bash
sudo apt update
sudo apt install -y build-essential libsqlite3-dev sqlite3 socat screen
```

**Lógica do Projeto**

1. **Serial UI & Menu**: após iniciar, o software exibe um menu na serial COM para:
   - Cadastrar usuário (nome, senha, tipo e permissões de sala).
   - Listar usuários.
   - Listar eventos (exige senha de administrador).
   - Liberar porta 1 ou 2 (validação de senha e permissão).
2. **Banco de Dados**: usuários e eventos são armazenados em SQLite (`acesso.db`).
3. **Modbus RTU**: comandos para abertura/fechamento de portas via registros 0x34 (porta 1) e 0x35 (porta 2); valores 0xFF (abrir) e 0x00 (fechar).
4. **Webserver**: roda em localhost:8080, mostra eventos cadastrados em tabela HTML.

**Simulação das Portas Serial e Modbus**

Para criar pares de terminais pseudo-TTY e simular interfaces COM:

```bash
# Primeiro par (serial UI ↔ cliente)
socat -d -d pty,raw,echo=0 pty,raw,echo=0
# Segundo par (Modbus ↔ controlador)
socat -d -d pty,raw,echo=0 pty,raw,echo=0
```

Após cada comando, anote os dispositivos gerados em `/dev/pts/X` e `/dev/pts/Y`. Exemplo de atribuição:

- `/dev/pts/3` → usado como porta serial da UI no código.
- `/dev/pts/4` → conectado via `screen` para interação do usuário.
- `/dev/pts/5` → usado como porta Modbus no código.
- `/dev/pts/6` → monitor opcional do lado Modbus.

Para acessar a UI serial:

```bash
screen /dev/pts/4 9600
```

**Compilação e Execução**

Na raiz do projeto:

```bash
make clean && make
./gerenciamento_de_acesso
```

- `make clean` remove binários antigos.
- `make` compila o executável `gerenciamento_de_acesso`.
- Ao rodar, o menu aparece no terminal conectado via `screen`.

Para acessar o painel web de eventos:

Abra no navegador: `http://localhost:8080`

---

*Desenvolvido para desafio de Linux embarcado usando C, SQLite e Modbus RTU.*

