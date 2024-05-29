#include <gtk/gtk.h>
#include "core.c"

FOOD food[100];
COMBO combo[100];
FILE* dict;
FILE* input;
FILE* output;
int foodCount, comboCount, orderCount, w1, w2;
ORDER* order;

/*
static int time;

int get_time(){
    //TODO
}

ORDER* order_add(char* name){
    ORDER* orderPtr=malloc(sizeof(ORDER));
    for(int j=0;j<20;j++) orderPtr->foodIndex[j]=-1;
    orderPtr->out=0;
    orderPtr->in=getTime();
    int temp=searchInFood(name);
    if(temp!=-1){
        order[i].count=1;
        order[i].foodIndex[0]=temp;
    } else{
        temp=searchInCombo(name);
        for(int j=0;j<combo[temp].count;j++) order[i].foodIndex[j]=combo[temp].foodIndex[j];
        order[i].count=combo[temp].count;
    }
    return orderPtr;
}
*/
static void clicked(GtkWidget* button){
    const gchar *name=gtk_button_get_label(GTK_BUTTON(button));
    //order_add(name);
    printf("%s", name);
}

static void create_menu_buttons(GtkWidget *grid){
    for(int i=0; i<foodCount; i++){
        GtkWidget *button=gtk_button_new_with_label(food[i].name);
        g_signal_connect(button, "clicked", G_CALLBACK(clicked), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, 0, i, 1, 1);
    }
    for(int i=0; i<foodCount; i++){
        GtkWidget *pBar=gtk_progress_bar_new();
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pBar), "test");
        gtk_widget_set_margin_top(pBar, 15);
        gtk_widget_set_size_request(pBar, -1, -1);
        gtk_grid_attach(GTK_GRID(grid), pBar, 1, i, 2, 1);
    }
    for(int i=0; i<comboCount; i++){
        GtkWidget *button=gtk_button_new_with_label(combo[i].name);
        gtk_widget_set_size_request(button, 20, -1);
        g_signal_connect(button, "clicked", G_CALLBACK(clicked), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, 3, i, 1, 1);
    }
}

static void order_pane_create(GtkWidget* grid){
    //int time=get_time();
    int time=1110;
    int pane_size=500;
    GtkWidget* pane=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget* left_pane=gtk_frame_new("pending");
    GtkWidget* right_pane=gtk_frame_new("done");
    GtkWidget* left_grid=gtk_grid_new();
    GtkWidget* right_grid=gtk_grid_new();
    gtk_frame_set_child(GTK_FRAME(left_pane), left_grid);
    gtk_frame_set_child(GTK_FRAME(right_pane), right_grid);
    gtk_paned_set_start_child(GTK_PANED(pane), left_pane);
    gtk_paned_set_end_child(GTK_PANED(pane), right_pane);
    gtk_paned_set_position(GTK_PANED(pane), pane_size/2);
    gtk_grid_attach(GTK_GRID(grid), pane, 2, 1, 4, 4);
    gtk_widget_set_size_request(pane, pane_size, -1);
    for(int i=0;i<orderCount;i++){
        char order_content[60];
        sprintf(order_content, "%d  %s", i, order[i].name);
        GtkWidget* label=gtk_label_new(order_content);
        if(order[i].out>time){
            gtk_grid_attach(GTK_GRID(left_grid), label, 0, i, 1, 1);
        } else{
            gtk_grid_attach(GTK_GRID(right_grid), label, 0, i, 1, 1);
            GtkCssProvider *css_provider=gtk_css_provider_new();
            gtk_css_provider_load_from_string(css_provider, ".fail { color: red; } .success { color: green; }");
            gtk_style_context_add_provider_for_display(gtk_widget_get_display(pane), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            if(order[i].out==-1){
                gtk_widget_add_css_class(label, "fail");
            } else{
                gtk_widget_add_css_class(label, "success");
            }
        }
    }
}

static void create_all(GtkWidget *grid){

    gtk_grid_set_row_homogeneous(GTK_GRID(grid), true);
    //gtk_grid_set_column_homogeneous(GTK_GRID(grid), true);

    GtkWidget* menu_grid=gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(menu_grid), true);
    gtk_grid_attach(GTK_GRID(grid), menu_grid, 0, 1, 1, 4);
    create_menu_buttons(menu_grid);

    GtkWidget* timeLabel=gtk_label_new(NULL);
    gtk_widget_add_css_class(timeLabel, "timeLabel");
    gtk_label_set_text(GTK_LABEL(timeLabel), "07:00:00"); //TODO
    GtkCssProvider *cssProvider=gtk_css_provider_new();
    gtk_css_provider_load_from_string(cssProvider, ".timeLabel { font-size: 40px; }");
    gtk_style_context_add_provider_for_display(gtk_widget_get_display(grid), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    //char* time=get_time();
    //gtk_label_set_text(GTK_LABEL(timeLabel), time);
    gtk_grid_attach(GTK_GRID(grid), timeLabel, 0, 0, 2, 1);

    GtkWidget* scale=gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, 10, 0.1);
    gtk_grid_attach(GTK_GRID(grid), scale, 3, 0, 1, 1);
    gtk_widget_set_size_request(scale, 100, -1);

    order_pane_create(grid);
}

static void app_startup(GApplication* app){
    GtkWidget *window=gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    GtkWidget *grid=gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);
    create_all(grid);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char** argv){
    fileOpen();
    GtkApplication *app;
    app=gtk_application_new("com.example.BUPTMcDonalds", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "startup", G_CALLBACK(app_startup), NULL);
    int status=g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    fileClose();
    return status;
}