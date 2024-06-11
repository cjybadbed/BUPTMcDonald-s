#include <gtk/gtk.h>
#include "core.c"
#include <math.h>

FOOD food[100];
COMBO combo[100];
FILE* dict;
FILE* input;
FILE* output;
FILE* nos;  //new order stream
int foodCount, comboCount, w1, w2;
int closeSec=-1, openSec=-1, orderCount=0;
GtkApplication *app;

FOOD food[MAX_FOOD];
COMBO combo[MAX_COMBO];
ORDER order[MAX_ORDER];

int get_time(){
    guint sec=-1;
    if(!gtk_application_get_windows(app)) return -1;
    GtkWindow* window=GTK_WINDOW(g_list_first(gtk_application_get_windows(app))->data);
    GtkWidget* grid_overall=gtk_window_get_child(window);
    GtkWidget* time_label=gtk_grid_get_child_at(GTK_GRID(grid_overall), 0, 0);
    char* time=(char*)gtk_label_get_text(GTK_LABEL(time_label));
    sec=time2sec(time);
    return sec;
}

static void clicked(GtkWidget* button){
    int curr_time=get_time();
    gchar *name=(gchar*)gtk_button_get_label(GTK_BUTTON(button));
    singleOrderInputAndHandle(name, curr_time);
}

static void menu_create(GtkWidget *grid){
    for(int i=0; i<foodCount; i++){
        GtkWidget *button=gtk_button_new_with_label(food[i].name);
        g_signal_connect(button, "clicked", G_CALLBACK(clicked), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, 0, i, 1, 1);
    }
    for(int i=0; i<foodCount; i++){
        GtkWidget *pBar=gtk_progress_bar_new();
        //gtk_widget_set_margin_top(pBar, 5);
        gtk_widget_set_size_request(pBar, -1, -1);
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(pBar), TRUE);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pBar), "0");
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
    int paned_size=600;
    GtkWidget* paned=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    GtkWidget* left_paned=gtk_frame_new("pending");
    GtkWidget* right_paned=gtk_frame_new("done");
    GtkWidget* left_grid=gtk_grid_new();
    GtkWidget* right_grid=gtk_grid_new();
    gtk_frame_set_child(GTK_FRAME(left_paned), left_grid);
    gtk_frame_set_child(GTK_FRAME(right_paned), right_grid);
    gtk_paned_set_start_child(GTK_PANED(paned), left_paned);
    gtk_paned_set_end_child(GTK_PANED(paned), right_paned);
    gtk_paned_set_position(GTK_PANED(paned), paned_size/2);
    gtk_grid_attach(GTK_GRID(grid), paned, 2, 1, 4, 4);
    gtk_widget_set_size_request(paned, paned_size, -1);
    GtkCssProvider *css_provider=gtk_css_provider_new();
    gtk_css_provider_load_from_string(css_provider, ".fail { color: red; } .success { color: green; } ");
    gtk_style_context_add_provider_for_display(gtk_widget_get_display(paned), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

}

static void create_all(GtkWidget *grid){

    //gtk_grid_set_row_homogeneous(GTK_GRID(grid), true);
    //gtk_grid_set_column_homogeneous(GTK_GRID(grid), true);

    GtkWidget* menu_grid=gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(menu_grid), true);
    gtk_grid_attach(GTK_GRID(grid), menu_grid, 0, 1, 1, 4);
    menu_create(menu_grid);

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
    gtk_window_set_title(GTK_WINDOW(window), "麦当劳点餐模拟系统");
    GtkWidget *grid=gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);
    create_all(grid);
    gtk_window_present(GTK_WINDOW(window));

}

static void order_refresh(){
    GtkWindow* window=GTK_WINDOW(g_list_first(gtk_application_get_windows(app))->data);
    GtkWidget* grid_overall=gtk_window_get_child(window);
    GtkWidget* paned=gtk_grid_get_child_at(GTK_GRID(grid_overall), 2, 1);
    GtkWidget* left_paned=gtk_paned_get_start_child(GTK_PANED(paned));
    GtkWidget* right_paned=gtk_paned_get_end_child(GTK_PANED(paned));
    GtkWidget* left_grid=gtk_frame_get_child(GTK_FRAME(left_paned));
    GtkWidget* right_grid=gtk_frame_get_child(GTK_FRAME(right_paned));
    int curr_time=get_time();
    gtk_grid_remove_column(GTK_GRID(left_grid), 0);
    gtk_grid_remove_column(GTK_GRID(right_grid), 0);
    if(orderCount){
        for(int i=0;i<orderCount;i++){
            char order_content[60];
            if(order[i].out==-1) sprintf(order_content, "%d  %s", i+1, order[i].name);
            else sprintf(order_content, "%d  %s  %s", i+1, order[i].name, sec2time(order[i].out));
            GtkWidget* label=gtk_label_new(order_content);
            if(order[i].out>curr_time){
                gtk_grid_attach(GTK_GRID(left_grid), label, 0, i, 1, 1);
            } else{
                gtk_grid_attach(GTK_GRID(right_grid), label, 0, i, 1, 1);
                if(order[i].out==-1){
                    gtk_widget_add_css_class(label, "fail");
                } else{
                    gtk_widget_add_css_class(label, "success");
                }
            }
        }
    }
}

static void progress_refresh(){
    GtkWindow* window=GTK_WINDOW(g_list_first(gtk_application_get_windows(app))->data);
    GtkWidget* grid_overall=gtk_window_get_child(window);
    GtkWidget* menu_grid=gtk_grid_get_child_at(GTK_GRID(grid_overall), 0, 1);
    int curr_time=get_time();
    for(int i=0;i<foodCount;i++){
        GtkWidget* pBar=gtk_grid_get_child_at(GTK_GRID(menu_grid), 1, i);
        int _storage, _progress;
        if(curr_time>=food[i].captime){
            _storage=food[i].cap;
            _progress=0;
        } else{
            if(curr_time>=(food[i].captime-food[i].cap*food[i].time)){
                _storage=(curr_time-(food[i].captime-food[i].cap*food[i].time))/food[i].time;
                _progress=(curr_time-(food[i].captime-food[i].cap*food[i].time))%food[i].time;
            } else{
                _storage=0;
                _progress=food[i].time-(food[i].captime-curr_time)%food[i].time;
            }
        }
        gchar storage[3];
        gdouble progress;
        sprintf(storage, "%d", _storage);
        progress=(gdouble)_progress/food[i].time;
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pBar), storage);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pBar), progress);
    }
}

gboolean ten_ms_handler(GtkWidget* grid_overall){ //TODO
    guint curr_time=get_time();
    if(curr_time==-1) {
        orderOutput(output);
        return FALSE;
    }
    GtkWidget* scale=gtk_grid_get_child_at(GTK_GRID(grid_overall), 3, 0);
    GtkWidget* time_label=gtk_grid_get_child_at(GTK_GRID(grid_overall), 0, 0);
    if(curr_time>=time2sec("23:59:59")){
        gtk_label_set_text(GTK_LABEL(time_label), "已结束");
        GtkWidget* window=gtk_widget_get_parent(grid_overall);
        GtkAlertDialog* day_end_alert=gtk_alert_dialog_new("You've reached the end of the day!");
        gtk_alert_dialog_show(day_end_alert, GTK_WINDOW(window));
        orderOutput(output);
        return FALSE;
    }
    gdouble time_factor=exp(gtk_range_get_value(GTK_RANGE(scale)));
    static guint ms_accumulated=0;
    ms_accumulated+=10*time_factor;
    if(ms_accumulated>=1000){
        gtk_label_set_text(GTK_LABEL(time_label), sec2time(curr_time+ms_accumulated/1000));
        progress_refresh();
        order_refresh();
        if(singleOrderRead(curr_time)) ithOrderHandle(orderCount-1);
        ms_accumulated%=1000;
    } else return TRUE;
    return TRUE;
}

static void app_activate(){
    GtkWindow* window=GTK_WINDOW(g_list_first(gtk_application_get_windows(app))->data);
    GtkWidget* grid_overall=gtk_window_get_child(window);
    g_timeout_add(10, G_SOURCE_FUNC(ten_ms_handler), grid_overall);
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