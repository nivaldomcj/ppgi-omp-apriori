#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>

using namespace std;

int main()
{
    // Parâmetros do Apriori
    //int aMinSupport = 5;
    int aMinSupport = 5;
    //int aMinConfidence = 30;
    int aMinConfidence = 5;

    /* 0) Leitura da instância */
    int aNumTransactions = 0;
    int aNumProducts = 0;
    int aNumSales = 0;

    string line;
    ifstream file("dataset1.txt");

    if (!file.is_open())
    {
        cout << "Nao foi possivel executar o arquivo!\n";
        return -1;
    }

    // Obtêm as informações da instância
    getline(file, line);
    aNumSales = stoi(line);

    getline(file, line);
    aNumProducts = stoi(line);

    getline(file, line);
    aNumTransactions = stoi(line);

    cout << "read instance info\n";

    // Inicializa a matriz de produtos
    int **aSales = new int *[aNumSales];
    for (int i = 0; i < aNumSales; i++)
        aSales[i] = new int[2];
    for (int i = 0; i < aNumSales; i++)
        for (int j = 0; j < 2; j++)
            aSales[i][j] = 0;

    // Adiciona os valores da instância à matriz
    for (int i = 0; i < aNumSales; i++)
        for (int j = 0; j < 2; j++)
            file >> aSales[i][j];

    cout << "added matrix instance\n";

    // início da execução (tempo)
    double codeStartTime = clock();

    /* 2) Obtêm uma matriz de fator de suporte de todos os produtos (aSupportProducts) */
    int aSupportProducts[aNumProducts + 1] = {0}; // todos os "suportes" de todos os produtos
    int countMinSupport = 0;                      // número de produtos que tem o suporte mínimo

    for (int i = 0; i <= aNumProducts; i++)
    {
        // precisamos contar a frequência que esse produto aparece no dataset
        int iProductFrequency = 0;

        for (int j = 0; j < aNumSales; j++)
        {
            // incrementa a frequência se o produto tem a frequência
            if (aSales[j][1] == i)
                iProductFrequency++;
        }

        // calculamos o suporte desse produto
        aSupportProducts[i] = round(100 * (iProductFrequency) / aNumTransactions);

        // contamos quantos produtos tem o suporte mínimo
        if (aSupportProducts[i] >= aMinSupport)
            countMinSupport++;
    }

    cout << "done matrix support\n";

    /* 3) De todos os produtos, precisamos de uma matriz com os primeiros produtos
       que alcançaram suporte mínimo [idProduto, support] (aMinSupportProducts) */
    // inicializa a matriz dinâmica
    int **aFirstMinSupportProducts = new int *[countMinSupport];
    for (int i = 0; i < countMinSupport; i++)
        aFirstMinSupportProducts[i] = new int[2];

    // preenche os produtos que alcançaram suporte mínimo
    for (int i = 0, j = 0; i <= aNumProducts; i++)
    {
        if (aSupportProducts[i] >= aMinSupport)
        {
            aFirstMinSupportProducts[j][0] = i;                   // ID do produto
            aFirstMinSupportProducts[j][1] = aSupportProducts[i]; // Fator de Suporte
            j++;
        }
    }

    cout << "done min support products\n";

    for (int i = 0; i < countMinSupport; i++)
    {
        cout << aFirstMinSupportProducts[i][0] << "-> " << aFirstMinSupportProducts[i][1] << " | ";
    }

    /* 4) Verifica as associações dos primeiros produtos que alcançaram suporte mínimo */
    int **allMiningRulesFound = new int *[aNumSales]; // (productIdX, productIdY, frequencyProductX, frequencyProductY)
    for (int i = 0; i < aNumSales; i++)
        allMiningRulesFound[i] = new int[4];

    for (int i = 0; i < aNumSales; i++)
        for (int j = 0; j < 4; j++)
            allMiningRulesFound[i][j] = 0;

    int nextRuleFoundIndex = 0;

    cout << "done mining rules table initiation\n";

    for (int i = 0; i < countMinSupport; i++)
    {
        // obtemos o próximo item que tinha o suporte mínimo
        int currentMinProductID = aFirstMinSupportProducts[i][0];
        int currentMinProductSupport = aFirstMinSupportProducts[i][1];

        for (int j = 0; j < aNumSales; j++)
        {
            int nextIdTransation = aSales[j][0];
            int nextIdProduct = aSales[j][1];

            // se o próximo produto é o mesmo do menor, não faz sentido comparar
            if (nextIdProduct == currentMinProductID)
                continue;

            // ignore se nessa transação desse item NÃO contem o item de suporte mínimo
            bool productsInTransaction = false;
            for (int k = 0; k < aNumSales; k++)
            {
                if (aSales[k][0] == nextIdTransation && aSales[k][1] == currentMinProductID)
                    productsInTransaction = true;
            }
            if (!productsInTransaction)
                continue;

            // contamos a frequência e suporte desse produto quando aparece TAMBÉM com o item de suporte mínimo (freqXY)
            int nextFrequencyTogether = 0;

            for (int k = 0; k < aNumSales; k++)
            {
                if (aSales[k][1] == nextIdProduct)
                {
                    for (int l = 0; l < aNumSales; l++)
                    {
                        if (aSales[l][0] == aSales[k][0] && aSales[l][1] == currentMinProductID)
                        {
                            nextFrequencyTogether++;
                            break;
                        }
                    }
                }
            }

            int nextSupportTogether = round(100 * (nextFrequencyTogether) / aNumTransactions);

            // descartamos as regras que não interessam (que tenham a menor confiança)
            double xyConfidence = ((double)nextSupportTogether / (double)currentMinProductSupport);

            if (xyConfidence < ((double)aMinConfidence / 100))
                continue;

            // adicionamos essa regra a todas as regras de mineração encontradas
            // primeiro, verificamos se não foi adicionada previamente essa regra
            bool ruleAddedPreviously = false;
            for (int k = 0; k < 21; k++)
            {
                if (allMiningRulesFound[k][0] == currentMinProductID && allMiningRulesFound[k][1] == nextIdProduct)
                {
                    ruleAddedPreviously = true;
                    break;
                }
            }

            if (!ruleAddedPreviously)
            {
                allMiningRulesFound[nextRuleFoundIndex][0] = currentMinProductID;
                allMiningRulesFound[nextRuleFoundIndex][1] = nextIdProduct;
                allMiningRulesFound[nextRuleFoundIndex][2] = currentMinProductSupport;
                allMiningRulesFound[nextRuleFoundIndex][3] = nextSupportTogether;
                nextRuleFoundIndex++;
            }
        }
    }

    cout << "done mining rules\n";

    // removemos o aFirstMinSupportProducts, não vamos precisar mais
    delete aFirstMinSupportProducts;

    /* 5) Exibe as regras encontradas */
    for (int i = 0; i < aNumSales; i++)
    {
        // não exiba regras não preenchidas (removidas)
        if (allMiningRulesFound[i][0] == 0)
            continue;

        cout << "x=" << allMiningRulesFound[i][0] << " y=" << allMiningRulesFound[i][1];
        cout << " freqX=" << allMiningRulesFound[i][2] << " freqXY=" << allMiningRulesFound[i][3] << endl;
    }

    cout << "done show mining rules\n";

    // fim da execução (tempo)
    double codeFinishTime = clock();
    double codeDuration = ((double)(codeFinishTime - codeStartTime) / (double)CLOCKS_PER_SEC);

    cout << "\nTempo total: " << codeDuration << " segundos" << endl;

    return 0;
}