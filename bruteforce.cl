// Les macros (rotate, choose, majority, etc.) viendront ici tout en haut !
#define rotate(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define sigma0(x) (rotate(x,2) ^ rotate(x,13) ^ rotate(x, 22))
#define sigma1(x) (rotate(x,6) ^ rotate(x, 11) ^ rotate(x,25))
#define low_sigma0(x) (rotate(x, 7) ^ rotate(x, 18) ^ (x >> 3))
#define low_sigma1(x) (rotate(x, 17) ^ rotate(x, 19) ^ (x >> 10))
#define choose(x,y,z) ((x & y) ^ (~x & z))
#define majority(x,y,z) ((x & y) | (x & z) | (y & z))

__kernel void bruteforce_sha256(
    __constant uint* K,             // Le tableau des 64 constantes (fourni une seule fois pour tous)
    __constant uint* target_hash,   // Le hash secret qu'on cherche à casser (ex: 8 entiers)
    int password_size,              // La taille du mot de passe qu'on teste (ex: 3 pour "aba")
    __global ulong* result_id         // La boîte aux lettres où l'ouvrier gagnant mettra son ID
) {

    uint h0 = 0x6a09e667;
    uint h1 = 0xbb67ae85;
    uint h2 = 0x3c6ef372;
    uint h3 = 0xa54ff53a;
    uint h4 = 0x510e527f;
    uint h5 = 0x9b05688c;
    uint h6 = 0x1f83d9ab;
    uint h7 = 0x5be0cd19;

    uint a = h0;
    uint b = h1;
    uint c = h2;
    uint d = h3;
    uint e = h4;
    uint f = h5;
    uint g = h6;
    uint h = h7;

    // 1. L'ouvrier regarde son badge (son numéro unique)
    ulong id = get_global_id(0);

    // 2. Génération du mot de passe (id_to_password)
    char abc[] = "abcdefghijklmnopqrstuvwxyz";
    char password[32] = {0}; 
    
    // --> C'EST ICI QUE TU VAS METTRE TA BOUCLE POUR GÉNÉRER LE MOT DE PASSE <--
    ulong temp_id = id;
    for (int i = password_size - 1; i >= 0; i--) {
        password[i] = abc[temp_id % 26];
        temp_id = temp_id / 26;
    }

    // 3. Préparation du bloc W et Padding
    uint W[64];
    // --> C'EST ICI QUE TU VAS METTRE TON CODE DE PADDING <--
    // 1. On nettoie tout (les 16 cases)
    for (int i = 0; i < 16; i++){
        W[i] = 0x00000000; // Pas besoin de "i *", 0 suffit ;
    }

    // 2. On traite les lettres UNE PAR UNE. // Pas de perte !
    for (int i = 0; i < password_size; i++){
        int index_boite = i / 4;
        int decalage = (3 - (i % 4)) * 8;
        W[index_boite] |= ((uint)password[i]) << decalage;
    }

    // 3. On ajoute le 0x80 à sa position exacte
    int index_boite_fin = password_size / 4;
    int decalage_fin = (3 - (password_size % 4)) * 8;
    W[index_boite_fin] |= 0x80 << decalage_fin;

    // 4. On place la taille en bits à la fin
    W[15] = password_size * 8;

    // 4. Le moteur SHA-256
    // --> C'EST ICI QUE TU METTRAS L'EXTENSION ET LA COMPRESSION <--

    // les 48 suivants, W[i] = low_sigma1(W[i - 2]) + W[i - 7] + low_sigma0(W[i - 15]) + W[i - 16]
    for (int i = 16; i < 64; i++){
        W[i] = low_sigma1(W[i - 2]) + W[i - 7] + low_sigma0(W[i - 15]) + W[i - 16];
    }
    for (int i = 0; i < 64; i++){
        uint T1 = h + sigma1(e) + choose(e,f,g) + K[i] + W[i];
        uint T2 = sigma0(a) + majority(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }
    
    h0 = h0 + a;
    h1 = h1 + b;
    h2 = h2 + c;
    h3 = h3 + d;
    h4 = h4 + e;
    h5 = h5 + f;
    h6 = h6 + g;
    h7 = h7 + h;

    // 5. La vérification finale (Le moment de vérité)
    // On compare les 8 morceaux du hash (h0 à h7) avec le target_hash
    if (h0 == target_hash[0] && h1 == target_hash[1] && h2 == target_hash[2] && h3 == target_hash[3] &&
        h4 == target_hash[4] && h5 == target_hash[5] && h6 == target_hash[6] && h7 == target_hash[7]) {
        
        // BINGO ! // L'ouvrier a trouvé. Il écrit son ID dans la mémoire globale pour avertir le CPU.
        *result_id = id;
    }
}