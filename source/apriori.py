def read_sales(filename):
    sales = []

    with open(filename, "r") as file:
        for line in file:
            line = line.split(",")

            if line[0] == "CustomerID":
                continue

            sales.append([int(line[0]), int(line[1])])

    return sales


def get_number_distinct_transactions(sales):
    distinct = set()

    for i in range(len(sales)):
        transactionID = sales[i][0]
        distinct.add(transactionID)

    return len(distinct)


def get_number_distinct_products(sales):
    distinct = set()

    for i in range(len(sales)):
        productID = sales[i][1]
        distinct.add(productID)

    return len(distinct)


def get_products_support_factor_list(sales, num_sales, num_products, num_transactions):
    # obtemos a frequência que o produto aparece no dataset
    product_frequency = [0 for i in range(num_products)]

    for i in range(num_sales):
        product_frequency[sales[i][1]] += 1

    # calcula o suporte pra cada produto
    support_factor = [0 for i in range(num_products)]

    for i in range(num_products):
        support_factor[i] = round(100 * (product_frequency[i] / num_transactions))

    return support_factor


def get_first_products_min_support(support_factor, num_products, min_support):
    min_support_products = []

    for i in range(num_products):
        if support_factor[i] >= min_support:
            min_support_products.append([i, support_factor[i]])
    
    return min_support_products # (id, support)


def get_association_products_min_support(sales, products_min_support, num_transactions, min_confidence):
    miningRulesFound = []

    for i in range(len(products_min_support)):
        pid, psupport = products_min_support[i]

        for j in range(len(sales)):
            ntransaction, nid = sales[j]

            if nid == pid:
                continue

            transactionHaveMinSupportItem = False

            for k in range(len(sales)):
                if sales[k][0] == ntransaction and sales[k][1] == pid:
                    transactionHaveMinSupportItem = True
                    break
            
            if not transactionHaveMinSupportItem:
                continue

            nextFrequencyTogether = 0

            # for k in range(len(sales)):
            #     if sales[k][1] == nid:
            #         for l in range(len(sales)):
            #             if sales[l][0] == sales[k][0] and sales[l][1] == pid:
            #                 nextFrequencyTogether += 1
            #                 break

            id_transactions_have_nid = []

            for k in range(len(sales)):
                if sales[k][1] == nid:
                    id_transactions_have_nid.append(sales[k][0])

            for k in range(len(sales)):
                if sales[k][0] in id_transactions_have_nid:
                    if sales[k][1] == pid:
                        nextFrequencyTogether += 1
            
            nextSupportTogether = round(100 * (nextFrequencyTogether / num_transactions))

            xyConfidence = float(nextSupportTogether) / float(psupport)

            if xyConfidence < (min_confidence / 100):
                continue

            ruleAddedPreviously = False
            for id1, id2, _, _ in miningRulesFound:
                if id1 == nid and id2 == pid:
                    ruleAddedPreviously = True
                    break

                if id1 == pid and id2 == nid:
                    ruleAddedPreviously = True
                    break
            
            if not ruleAddedPreviously:
                miningRulesFound.append([pid, nid, psupport, nextSupportTogether])

    return miningRulesFound
            


def apriori(sales, min_support, min_confidence):
    num_sales = len(sales)
    num_transactions = get_number_distinct_transactions(sales)
    num_products = get_number_distinct_products(sales)

    print(num_sales, num_transactions, num_products)

    support_factor = get_products_support_factor_list(sales, num_sales, num_products, num_transactions)

    first_products_min_support = get_first_products_min_support(support_factor, num_products, min_support)

    print(first_products_min_support)

    associations = get_association_products_min_support(sales, first_products_min_support, num_transactions, min_confidence)

    print(associations)

sales = read_sales("sales.csv")
apriori(sales, 10, 60)