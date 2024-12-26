#ifndef ASK3_H
#define ASK3_H

#define NUM_CUSTOMER_ORDERS 10
#define CUSTOMERS_NUM 5
#define ITEM_PER_PRODUCT  2
#define PRODUCTS_NUMBER 20
#define ORDER_SUCCESS "Purchase completed, your total is : %.2f.\n"
#define ORDER_FAILED "Purchase unsuccesful, due to stock issues.\n"

struct catalog {
    char description[50];
    float price;
    int  item_count;
    int pieces_sold; //requested and sold succesfully
    int pieces_requested; // requested but werent available
    int customers_not_serviced[CUSTOMERS_NUM]; 
}products;

void customerMsg(float server_response){
      if (server_response < 1){
        printf(ORDER_FAILED);
    }
    else {
        printf(ORDER_SUCCESS,server_response);
    }
}

void server(int fd_client_to_server[CUSTOMERS_NUM][2], int fd_server_to_client[CUSTOMERS_NUM][2]){

    struct catalog products[PRODUCTS_NUMBER] = {
        {"Smartphone",155.24,ITEM_PER_PRODUCT},//1,
        {"Gaming Monitor",105.99,ITEM_PER_PRODUCT},//2
        {"Smart TV",356.64,ITEM_PER_PRODUCT},//3
        {"Home Cinema",2155.56,ITEM_PER_PRODUCT},//4
        {"Laptop",843.82,ITEM_PER_PRODUCT},//5
        {"Personal Computer",155.24,ITEM_PER_PRODUCT},//6
        {"Headphones",99.99,ITEM_PER_PRODUCT},//7
        {"Wireless Buds",64.37,ITEM_PER_PRODUCT},//8
        {"Fridge",475.27,ITEM_PER_PRODUCT},//9
        {"Oven",574.62,ITEM_PER_PRODUCT},//10
        {"Tablet",196.34,ITEM_PER_PRODUCT},//11
        {"Camera",426.24,ITEM_PER_PRODUCT},//12
        {"Printer",145.86,ITEM_PER_PRODUCT},//13
        {"Laptop Case",46.24,ITEM_PER_PRODUCT},//14
        {"Speakers",275.74,ITEM_PER_PRODUCT},//15
        {"Microphone",167.63,ITEM_PER_PRODUCT},//16
        {"USB Drive",17.34,ITEM_PER_PRODUCT},//17
        {"Wireless Mouse",26.65,ITEM_PER_PRODUCT},//18
        {"Wireless Keyboard",29.63,ITEM_PER_PRODUCT},//19
        {"Bluetooth Speaker",93.63,ITEM_PER_PRODUCT},//20
    };

    for (int i=0;i<PRODUCTS_NUMBER;i++){ //Arxikopoihsh twn plhroforiwn zhthshs gia ka8e proion
        products[i].pieces_sold = 0;
        products[i].pieces_requested = 0;

        for(int j=0;j<CUSTOMERS_NUM;j++){
            products[i].customers_not_serviced[j] = 0;
        }
    }

    int serve_request;
    int serve_request_answer = 304;
    int current_customer = 0;
    float total_revenue = 0;
    int completed_orders = 0;
    int incompleted_orders = 0;
    
    while (current_customer < CUSTOMERS_NUM){
        
        if (read(fd_client_to_server[current_customer][0],&serve_request,sizeof(int)) >0){

            if( serve_request == 303){
                //CUSTOMER ACCEPTED
                write(fd_server_to_client[current_customer][1],&serve_request_answer,sizeof(int));
                int product_ordered;
                printf("Customer %d accepted\n",current_customer+1);
                for (int i=0;i<NUM_CUSTOMER_ORDERS;i++){
                    
                    float order_total = 0;
                    float customer_ans =0;
                    int valid_order = 1;    

                    if (read(fd_client_to_server[current_customer][0],&product_ordered,sizeof(int))>0){
                    
                        usleep(50000);
                        order_total += products[product_ordered].price;
                        if (products[product_ordered].item_count != 0 ){
                            
                            products[product_ordered].pieces_sold++;
                            products[product_ordered].item_count --;
                            completed_orders ++;
                        }
                        else{
                            products[product_ordered].pieces_requested++;
                            valid_order = 0 ;
                            incompleted_orders ++;
                            products[product_ordered].customers_not_serviced[current_customer]=1;
                           
                        }
                    }
                    
                    customer_ans = order_total * valid_order;
                    total_revenue += customer_ans;
                                    
                    if (write(fd_server_to_client[current_customer][1],&customer_ans,sizeof(float)) == -1){
                        printf("Error with pipe writing to customer\n");
                    }
                }
            }
        }
        current_customer ++;          
    }
    close(fd_client_to_server[current_customer][0]);  
    close(fd_server_to_client[current_customer][1]);


    for(int i=0;i<CUSTOMERS_NUM;i++){
        waitpid(-1,NULL,0);
    }
    
    for (int i=0;i<PRODUCTS_NUMBER;i++){
        for(int j=0;j<CUSTOMERS_NUM;j++){
            if (products[i].customers_not_serviced[j]==1){
            }
        }
        printf("------\n");
    }

    int total_orders = incompleted_orders + completed_orders;
    printf("%d orders were made.\n%d of them were succesful and %d were unsuccesful.\n",total_orders,completed_orders,incompleted_orders);
    printf("Total shop revenue: %.2f.\n",total_revenue);

}

void client(int fd_client_to_server[2], int fd_server_to_client[2]){

    char resp[50];
    char b;
    int product_id_request;
    int request = 303;

    write(fd_client_to_server[1],&request,sizeof(int));
    int request_response;
    int served = 1;
    while (served){
        
        if (read(fd_server_to_client[0],&request_response,sizeof(int)) > 0 ){

            if ( request_response == 304 ){

                for (int i=0;i<NUM_CUSTOMER_ORDERS;i++){
                    
                    srand(time(NULL)+i);
                    product_id_request = rand()  % 20 ;
                    write(fd_client_to_server[1],&product_id_request,sizeof(int));
                    
                    sleep(1);
                    float server_response;
                    if (read(fd_server_to_client[0],&server_response,sizeof(float)) == -1){
                        printf("Error with pipe reading from server\n");
                        exit(1);
                    }
                    customerMsg(server_response);
                }
                close(fd_client_to_server[1]);
                close(fd_server_to_client[0]);
                served = 0;
                
            }
        }
    }
}
#endif