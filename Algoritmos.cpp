#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <queue>
using namespace std;
vector<int> algoritmoInocente(const string& texto, const string& patron) {
    vector<int> ocurrencias;
    for (int i = 0; i <= texto.length() - patron.length(); ++i) {
        int j;
        for (j = 0; j < patron.length(); ++j) {
            if (texto[i + j] != patron[j]) {
                break;
            }
        }
        if (j == patron.length()) {
            ocurrencias.push_back(i);
        }
    }
    return ocurrencias;
}
vector<int> algoritmoKMP(const string& texto, const string& patron) {
    vector<int> ocurrencias;
    vector<int> lps(patron.length(), 0);
    int j = 0;
    for (int i = 1; i < patron.length(); ++i) {
        if (patron[i] == patron[j]) {
            lps[i] = ++j;
        }
        else {
            if (j != 0) {
                j = lps[j - 1];
                --i;
            }
            else {
                lps[i] = 0;
            }
        }
    }
    j = 0;
    for (int i = 0; i < texto.length(); ++i) {
        if (texto[i] == patron[j]) {
            ++j;
        }
        else {
            if (j != 0) {
                j = lps[j - 1];
                --i;
            }
        }

        if (j == patron.length()) {
            ocurrencias.push_back(i - j + 1);
            j = lps[j - 1];
        }
    }
    return ocurrencias;
}
vector<int> algoritmoBoyerMoore(const string& texto, const string& patron) {
    vector<int> ocurrencias;
    int m = patron.length();
    int n = texto.length();
    vector<int> badChar(256, -1);
    for (int i = 0; i < m; ++i) {
        badChar[patron[i]] = i;
    }
    int s = 0;
    while (s <= n - m) {
        int j = m - 1;

        while (j >= 0 && patron[j] == texto[s + j]) {
            j--;
        }

        if (j < 0) {
            ocurrencias.push_back(s);
            s += (s + m < n) ? m - badChar[texto[s + m]] : 1;
        }
        else {
            s += max(1, j - badChar[texto[s + j]]);
        }
    }
    return ocurrencias;
}
int modPow(int base, int exponent, int modulo) {
    int result = 1;
    base %= modulo;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % modulo;
        }
        base = (base * base) % modulo;
        exponent /= 2;
    }
    return result;
}
vector<int> algoritmoRabinKarp(const string& texto, const string& patron) {
    vector<int> ocurrencias;
    int m = patron.length();
    int n = texto.length();
    int d = 256;
    int q = 101;
    int patronHash = 0;
    int textoHash = 0;
    for (int i = 0; i < m; ++i) {
        patronHash = (d * patronHash + patron[i]) % q;
        textoHash = (d * textoHash + texto[i]) % q;
    }

    for (int i = 0; i <= n - m; ++i) {
        if (patronHash == textoHash) {
            int j;
            for (j = 0; j < m; ++j) {
                if (texto[i + j] != patron[j]) {
                    break;
                }
            }
            if (j == m) {
                ocurrencias.push_back(i);
            }
        }
        if (i < n - m) {
            textoHash = (d * (textoHash - texto[i] * modPow(d, m - 1, q)) + texto[i + m]) % q;
            if (textoHash < 0) {
                textoHash += q;
            }
        }
    }
    return ocurrencias;
}
class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    vector<int> patternIndices;
    TrieNode* failure;
};
void buildTrie(const vector<string>& patrones, TrieNode* root) {
    for (int i = 0; i < patrones.size(); ++i) {
        TrieNode* node = root;
        for (char c : patrones[i]) {
            if (!node->children.count(c)) {
                node->children[c] = new TrieNode;
            }
            node = node->children[c];
        }
        node->patternIndices.push_back(i);
    }
}
void buildFailureFunction(TrieNode* root) {
    queue<TrieNode*> q;
    root->failure = root;
    for (auto& it : root->children) {
        it.second->failure = root;
        q.push(it.second);
    }
    while (!q.empty()) {
        TrieNode* current = q.front();
        q.pop();
        for (auto& it : current->children) {
            TrieNode* state = current->failure;
            char symbol = it.first;
            while (state != root && !state->children.count(symbol)) {
                state = state->failure;
            }
            if (state->children.count(symbol)) {
                state = state->children[symbol];
            }
            it.second->failure = state;
            q.push(it.second);
        }
    }
}
void ahoCorasick(const string& texto, const vector<string>& patrones) {
    TrieNode* root = new TrieNode;
    buildTrie(patrones, root);
    buildFailureFunction(root);
    TrieNode* current = root;
    for (int i = 0; i < texto.size(); ++i) {
        char c = texto[i];
        while (current != root && !current->children.count(c)) {
            current = current->failure;
        }
        if (current->children.count(c)) {
            current = current->children[c];
            for (int patternIndex : current->patternIndices) {
                cout << "Patrón encontrado en posición " << i - patrones[patternIndex].length() + 1 << ": " << patrones[patternIndex] << endl;
            }
        }
    }
}
void marcarOcurrenciasEnTexto(string& texto, const vector<int>& ocurrencias, const string& patron, char _marcador) {
    for (const int& ocurrencia : ocurrencias) {
        for (int i = 0; i < patron.length(); ++i) {
            texto[ocurrencia + i] = _marcador;
        }
    }
}

int main() {
    char marcador;
    string texto, patron;
    vector<string> patrones;
    int opcion;
    vector<vector<int>> ocurrencias;
    do {
        system("cls");
        cout << "1. Algoritmo Inocente (Brute Force)\n";
        cout << "2. Algoritmo de Knuth-Morris-Pratt (KMP)\n";
        cout << "3. Algoritmo de Boyer-Moore\n";
        cout << "4. Algoritmo de Rabin-Karp\n";
        cout << "5. Algoritmo de Aho-Corasick\n";
        cout << "6. Salir\n";
        cout << "Seleccione el algoritmo de búsqueda: ";
        cin >> opcion;
        cin.ignore();
        if (opcion == 6) {
            cout << "Gracias por usar el programa." << endl;
            return 0;
        }
        cout << "Ingrese el texto: ";
        getline(cin, texto);
        cout << "Ingrese el número de patrones a buscar: ";
        int numPatrones;
        cin >> numPatrones;
        cin.ignore();
        cout << "Ingrese los patrones uno por uno:" << endl;
        for (int i = 0; i < numPatrones; ++i) {
            cout << "Patrón N°" << i + 1 << ": ";
            getline(cin, patron);
            patrones.push_back(patron);
        }
        ocurrencias.clear();
        cout << "Ingrese el marcador para las ocurrencias (solo un símbolo): ";
        cin >> marcador;
        for (int i = 0; i < patrones.size(); ++i) {
            switch (opcion) {
            case 1:
                ocurrencias.push_back(algoritmoInocente(texto, patrones[i]));
                break;
            case 2:
                ocurrencias.push_back(algoritmoKMP(texto, patrones[i]));
                break;
            case 3:
                ocurrencias.push_back(algoritmoBoyerMoore(texto, patrones[i]));
                break;
            case 4:
                ocurrencias.push_back(algoritmoRabinKarp(texto, patrones[i]));
                break;
            case 5:
                ahoCorasick(texto, patrones);
                break;
            default:
                cout << "Opción no válida." << endl;
            }
        }
        for (int i = 0; i < patrones.size(); i++) {
            if (ocurrencias[i].empty()) {
                cout << "El patrón [" << patrones[i] << "] no se encontró en el texto." << endl;
            }
            else {
                cout << "El patrón [" << patrones[i] << "] se encontró en las siguientes posiciones:\n";
                string textoMarcado = texto;
                for (auto ocurrencia : ocurrencias[i]) {
                    cout << ocurrencia << " ";
                }
                cout << "\nReemplazo de texto por el símbolo [" << marcador << "]:\n";
                marcarOcurrenciasEnTexto(textoMarcado, ocurrencias[i], patrones[i], marcador);
                cout << textoMarcado << endl;
            }
        }
        system("pause");
    } while (true);
    return 0;
}
