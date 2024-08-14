#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #define CLEAR_SCREEN system("cls")
#else
    #define CLEAR_SCREEN system("clear")
#endif

// Structure to represent an item in an order
struct Item {
    char name[20];
    float price;
    int quantity;
};

// Structure to represent a customer order
struct Order {
    char customerName[50];
    char date[50];
    int itemCount;
    struct Item items[50];
};

// Function to print the invoice header
void printInvoiceHeader(const char customerName[50], const char date[30]) {
    printf("\n\n");
    printf("\t    ADV. Restaurant");
    printf("\n\t   -----------------");
    printf("\nDate: %s", date);
    printf("\nInvoice To: %s", customerName);
    printf("\n");
    printf("---------------------------------------\n");
    printf("Items\t\t");
    printf("Qty\t\t");
    printf("Total\t\t");
    printf("\n---------------------------------------");
    printf("\n\n");
}

// Function to print the details of each item in the invoice
void printInvoiceBody(const char itemName[30], int quantity, float price) {
    printf("%s\t\t", itemName);
    printf("%d\t\t", quantity);
    printf("%.2f\t\t", quantity * price);
    printf("\n");
}

// Function to print the invoice footer including totals and taxes
void printInvoiceFooter(float subtotal) {
    printf("\n");
    float discount = 0.1 * subtotal;
    float netTotal = subtotal - discount;
    float cgst = 0.09 * netTotal;
    float sgst = cgst;
    float grandTotal = netTotal + cgst + sgst;

    printf("---------------------------------------\n");
    printf("Sub Total\t\t\t%.2f", subtotal);
    printf("\nDiscount @10%s\t\t\t%.2f", "%", discount);
    printf("\n\t\t\t\t-------");
    printf("\nNet Total\t\t\t%.2f", netTotal);
    printf("\nCGST @9%s\t\t\t%.2f", "%", cgst);
    printf("\nSGST @9%s\t\t\t%.2f", "%", sgst);
    printf("\n---------------------------------------");
    printf("\nGrand Total\t\t\t%.2f", grandTotal);
    printf("\n---------------------------------------\n");
}

int main() {
    int option, numItems;
    struct Order currentOrder;
    struct Order savedOrder;
    char saveInvoiceChoice[2] = "y", continueFlag[2] = "y";
    char customerName[50];
    FILE *filePointer;

    // Main menu loop
    while (strcmp(continueFlag, "y") == 0) {
        CLEAR_SCREEN;
        float totalAmount = 0;
        int invoiceFound = 0;

        // Display menu
        printf("\t============ADV. RESTAURANT============");
        printf("\n\nPlease select your preferred operation");
        printf("\n\n1. Generate Invoice");
        printf("\n2. Show all Invoices");
        printf("\n3. Search Invoice");
        printf("\n4. Exit");
        printf("\n\nYour choice:\t");
        scanf("%d", &option);
        fgetc(stdin); // Consume newline character

        switch (option) {
            case 1:
                CLEAR_SCREEN;
                printf("\nPlease enter the name of the customer:\t");
                fgets(currentOrder.customerName, 50, stdin);
                currentOrder.customerName[strlen(currentOrder.customerName) - 1] = 0; // Remove newline character
                strcpy(currentOrder.date, __DATE__);
                printf("\nPlease enter the number of items:\t");
                scanf("%d", &numItems);
                currentOrder.itemCount = numItems;

                // Input items
                for (int i = 0; i < numItems; i++) {
                    fgetc(stdin); // Consume newline character
                    printf("\n\n");
                    printf("Please enter item %d:\t", i + 1);
                    fgets(currentOrder.items[i].name, 20, stdin);
                    currentOrder.items[i].name[strlen(currentOrder.items[i].name) - 1] = 0; // Remove newline character
                    printf("Please enter the quantity:\t");
                    scanf("%d", &currentOrder.items[i].quantity);
                    printf("Please enter the unit price:\t");
                    scanf("%f", &currentOrder.items[i].price);
                    totalAmount += currentOrder.items[i].quantity * currentOrder.items[i].price;
                }

                // Print and optionally save invoice
                printInvoiceHeader(currentOrder.customerName, currentOrder.date);
                for (int i = 0; i < currentOrder.itemCount; i++) {
                    printInvoiceBody(currentOrder.items[i].name, currentOrder.items[i].quantity, currentOrder.items[i].price);
                }
                printInvoiceFooter(totalAmount);

                printf("\nDo you want to save the invoice [y/n]:\t");
                fgets(saveInvoiceChoice, 2, stdin);
                fgetc(stdin); // Consume newline character

                if (saveInvoiceChoice[0] == 'y') {
                    filePointer = fopen("RestaurantBill.dat", "a+b");
                    if (filePointer == NULL) {
                        perror("Error opening file");
                        break;
                    }
                    if (fwrite(&currentOrder, sizeof(struct Order), 1, filePointer) != 1) {
                        printf("\nError saving invoice");
                    } else {
                        printf("\nInvoice successfully saved");
                    }
                    fclose(filePointer);
                }
                break;

            case 2:
                CLEAR_SCREEN;
                filePointer = fopen("RestaurantBill.dat", "rb");
                if (filePointer == NULL) {
                    perror("Error opening file");
                    break;
                }
                printf("\n  *****Your Previous Invoices*****\n");
                while (fread(&savedOrder, sizeof(struct Order), 1, filePointer) == 1) {
                    float total = 0;
                    printInvoiceHeader(savedOrder.customerName, savedOrder.date);
                    for (int i = 0; i < savedOrder.itemCount; i++) {
                        printInvoiceBody(savedOrder.items[i].name, savedOrder.items[i].quantity, savedOrder.items[i].price);
                        total += savedOrder.items[i].quantity * savedOrder.items[i].price;
                    }
                    printInvoiceFooter(total);
                }
                fclose(filePointer);
                break;

            case 3:
                printf("Enter the name of the customer:\t");
                fgets(customerName, 50, stdin);
                customerName[strlen(customerName) - 1] = 0; // Remove newline character
                CLEAR_SCREEN;
                filePointer = fopen("RestaurantBill.dat", "rb");
                if (filePointer == NULL) {
                    perror("Error opening file");
                    break;
                }
                printf("\t*****Invoice for %s*****", customerName);
                while (fread(&savedOrder, sizeof(struct Order), 1, filePointer) == 1) {
                    float total = 0;
                    if (strcmp(savedOrder.customerName, customerName) == 0) {
                        printInvoiceHeader(savedOrder.customerName, savedOrder.date);
                        for (int i = 0; i < savedOrder.itemCount; i++) {
                            printInvoiceBody(savedOrder.items[i].name, savedOrder.items[i].quantity, savedOrder.items[i].price);
                            total += savedOrder.items[i].quantity * savedOrder.items[i].price;
                        }
                        printInvoiceFooter(total);
                        invoiceFound = 1;
                    }
                }
                if (!invoiceFound) {
                    printf("Sorry, the invoice for %s does not exist", customerName);
                }
                fclose(filePointer);
                break;

            case 4:
                printf("\n\t\tBye Bye :)\n\n");
                exit(0);
                break;

            default:
                printf("Sorry, invalid option");
                break;
        }
        printf("\nDo you want to perform another operation?[y/n]:\t");
        fgets(continueFlag, 2, stdin);
        fgetc(stdin); // Consume newline character
    }
    printf("\n\t\tBye Bye :)\n\n");

    return 0;
}
