# RandomChat
Un app Android per chattare con sconosciuti. Sviluppata da RMC Inc. per l'Università di Napoli Federico II


## Requisiti
1. Il sistema deve gestire una random chat in cui i clients si collegano ad una stanza tematica e vengono messi in contatto con altri clients in maniera random. I client una, volta accoppiati, possono scambiarsi messaggi testuali fino alla chiusura da parte di una delle parti della chat. Ciascun client èidentificato pubblicamente da un nickname, scelto dall’utente.
2. Permettere all’utente di sapere quanti clients sono connessi in ogni stanza.
3. Permettere all’utente di mettersi in attesadiuna chat in determinata stanza.
4. Una volta stabilito il match permettere all’utente di scambiare messaggi con l’utente assegnato e di chiudere la conversazione in qualsiasi momento.
5. Non essere assegnato ad una medesima controparte nella stessa stanza in due assegnazioni consecutive.
6. Permettere l’invio di messaggi tramite i servizi di speech recognition del client android.
7. Prevedere un tempo massimo di durata di una conversazionedopo il quale si viene assegnati ad un'altra chat
### Requisiti non funzionali
Il server va realizzato in linguaggio C su piattaforma UNIX/Linuxe deve essere ospitato online su un server cloud. Il client va realizzato in linguaggio Java su piattaforma Android e fa utilizzo dei servizi di speech recognition. Client e server devono comunicare tramite socket TCP o UDP. Oltre alle system call UNIX, il server può̀utilizzare solo la libreria standard del C. Il server deve essere di tipo concorrente, ed in grado di gestire un numero arbitrario di client contemporaneamente. Il server effettua il log delle principali operazioni (nuove connessioni, sconnessioni, richieste da parte dei client) su standard output.


# Analisi

## UseCase
I requisiti funzionali elencati sopra possono essere riassunti nei seguenti casi d'uso.
![usecase](Documentation/UseCaseDiagram.svg)


## Classi di Analisi
![ClassDiagram](Documentation/ClassiAnalisiClient.svg)

- WelcomeController si occupa di far scegliere il nickname all'utente e di aprire la comunicazione con il server.
- RoomController si occupa di scaricare dal server le varie stanze e le fa selezionare all'utente
- Chatcontroller si occupa di scambiare i messaggi tra 2 utenti di una stessa stanza


# Progettazione
## ComponentDiagram e scomposizione Top-Down
![server state machine](Documentation/ComponentDiagramServer.svg)

- Il componente File loader si occupa di leggere le stanze da un file e metterle in una apposita struttura dati
- Il componente Finder si occupa di accoppiare gli utenti di una stessa stanza
- Il componente Messenger si occupa di far scambiare i messaggi a utenti accoppiati
- Server è il componente principale che gestisce gli altri e comunica con il Client

## Statechart
### Server
![server state machine](Documentation/RandomChatServer.svg)
### Client
![server state machine](Documentation/RandomChatClient.svg)
