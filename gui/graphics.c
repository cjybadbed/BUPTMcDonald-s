#include <gtk/gtk.h>
#include "core.c"
#include <math.h>

FOOD food[100];
COMBO combo[100];
FILE* dict;
FILE* input;
FILE* output;
FILE* nos;  //new order stream
int foodCount, comboCount, orderCount, w1, w2;
int closeSec=-1, openSec=-1;
ORDER* order;
GtkApplication *app;


guint get_time(){
    guint sec=-1;
    GtkWindow* window=GTK_WINDOW(g_list_first(gtk_application_get_windows(app))->data);
    GtkWidget* grid_overall=gtk_window_get_child(window);
    GtkWidget* time_label=gtk_grid_get_child_at(GTK_GRID(grid_overall), 0, 0);
    char* time=(char*)gtk_label_get_text(GTK_LABEL(time_label));
    sec=time2sec(time);
    return sec;
}

static void clicked(GtkWidget* button){
    const gchar *name=gtk_button_get_label(GTK_BUTTON(button));
    fprintf(stdout, "%s %s\n", sec2time(get_time()), name);
    //rewind(nos);
    //int i=singleOrderRead(nos);
    //printf("%s",order[i].name);
    //ithOrderHandle(i);
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

    //gtk_grid_set_row_homogeneous(GTK_GRID(grid), true);
    //gtk_grid_set_column_homogeneous(GTK_GRID(grid), true);

    GtkWidget* menu_grid=gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(menu_grid), true);
    gtk_grid_attach(GTK_GRID(grid), menu_grid, 0, 1, 1, 4);
    create_menu_buttons(menu_grid);

    GtkWidget* time_label=gtk_label_new(NULL);
    gtk_widget_add_css_class(time_label, "time_label");
    gtk_label_set_text(GTK_LABEL(time_label), sec2time(0));
    GtkCssProvider *cssProvider=gtk_css_provider_new();
    gtk_css_provider_load_from_string(cssProvider, ".time_label { font-size: 40px; }");
    gtk_style_context_add_provider_for_display(gtk_widget_get_display(grid), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_grid_attach(GTK_GRID(grid), time_label, 0, 0, 2, 1);

    GtkWidget* scale=gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 10, 0.1);
    gtk_range_set_value(GTK_RANGE(scale), 0);
    gtk_grid_attach(GTK_GRID(grid), scale, 3, 0, 2, 1);
    gtk_widget_set_size_request(scale, 100, -1);

    order_pane_create(grid);
}

static void app_startup(){
    GtkWidget *window=gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    GtkWidget *grid=gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);
    create_all(grid);
    gtk_window_present(GTK_WINDOW(window));

}

gboolean ten_ms_handler(GtkWidget* grid_overall){ //TODO
    guint curr_time=get_time();
    GtkWidget* scale=gtk_grid_get_child_at(GTK_GRID(grid_overall), 3, 0);
    GtkWidget* time_label=gtk_grid_get_child_at(GTK_GRID(grid_overall), 0, 0);
    if(curr_time>=time2sec("23:59:59")){
        gtk_label_set_text(GTK_LABEL(time_label), "不伺候了您内！");
        GtkWidget* window=gtk_widget_get_parent(grid_overall);
        GtkAlertDialog* day_end_alert=gtk_alert_dialog_new("You've reached the end of the day!");
        gtk_alert_dialog_show(day_end_alert, GTK_WINDOW(window));
        return FALSE;
    }
    gdouble time_factor=exp(gtk_range_get_value(GTK_RANGE(scale)));
    static guint ms_accumulated=0;
    ms_accumulated+=10*time_factor;
    if(ms_accumulated>=1000){
        gtk_label_set_text(GTK_LABEL(time_label), sec2time(curr_time+ms_accumulated/1000));
        //progress_refresh();
        //order_refresh();
        ms_accumulated%=1000;
    } else return TRUE;
    return TRUE;
}

static void app_activate(){
    GtkWindow* window=GTK_WINDOW(g_list_first(gtk_application_get_windows(app))->data);
    GtkWidget* grid_overall=gtk_window_get_child(window);
    g_timeout_add(10, G_SOURCE_FUNC(ten_ms_handler), grid_overall);
    printf("success\n");
}

int main(int argc, char** argv){
    fileOpen();
    app=gtk_application_new("com.cjybadbed.BUPTMcDonalds", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "startup", G_CALLBACK(app_startup), NULL);
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
    int status=g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    fileClose();
    return status;
}