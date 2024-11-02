#include "game_menu.h"

struct GameEngine {
    Gui* gui;
    NotificationApp* notifications;
    FuriPubSub* input_pubsub;
    FuriThreadId thread_id;
    GameEngineSettings settings;
    float fps;
};

bool game_menu_tutorial_selected = false;
bool game_menu_settings_selected = false;
bool game_menu_quit_selected = false;
FuriApiLock game_menu_exit_lock = NULL;

ViewHolder* view_holder;
Gui* gui;

void game_settings_menu_button_callback(void* game_manager, uint32_t index) {
    UNUSED(game_manager);
    if(index == 3) {
        //TODO Back to main menu or pause menu.
    }
    UNUSED(index);
}

void game_menu_button_callback(void* game_manager, uint32_t index) {
    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);
    UNUSED(game_manager);
    if(index == 0) {
        game_menu_tutorial_selected = false;
        game_menu_quit_selected = false;
        //TODO value for play game.
        notification_message(notifications, &sequence_success);
        api_lock_unlock(game_menu_exit_lock);
    } else if(index == 1) {
        game_menu_settings_selected = false;
        game_menu_quit_selected = false;
        game_menu_tutorial_selected = true;
        notification_message(notifications, &sequence_success);
        api_lock_unlock(game_menu_exit_lock);
    } else if(index == 2) {
        game_menu_tutorial_selected = false;
        game_menu_quit_selected = false;
        game_menu_settings_selected = true;
        //Settings menu
        notification_message(notifications, &sequence_single_vibro);

        api_lock_unlock(game_menu_exit_lock);
    } else if(index == 3) {
        //Quit
        game_menu_tutorial_selected = false;
        game_menu_settings_selected = false;
        game_menu_quit_selected = true;
        notification_message(notifications, &sequence_single_vibro);
        api_lock_unlock(game_menu_exit_lock);
    }
}

void game_menu_open(GameManager* game_manager, bool reopen) {
    UNUSED(reopen);
    game_menu_exit_lock = api_lock_alloc_locked();
    gui = furi_record_open(RECORD_GUI);

    /*if(reopen) {
        FURI_LOG_I("DEADZONE", "Hey, re-opening menu!");
        GameEngine* engine = game_manager_engine_get(game_manager);
        gui = engine->gui;
    }*/

    //Setup widget UI
    /*
     Widget* widget = widget_alloc();
    widget_add_string_element(widget, 0, 30, AlignCenter, AlignCenter, FontPrimary, "test center");

    ViewHolder* view_holder = view_holder_alloc();

    view_holder_attach_to_gui(view_holder, gui);
    view_holder_set_view(view_holder, widget_get_view(widget));*/

    Submenu* submenu = submenu_alloc();
    submenu_add_item(submenu, "PLAY GAME", 0, game_menu_button_callback, game_manager);
    submenu_add_item(submenu, "TUTORIAL", 1, game_menu_button_callback, game_manager);
    submenu_add_item(submenu, "SETTINGS", 2, game_menu_button_callback, game_manager);
    submenu_add_item(submenu, "QUIT", 3, game_menu_button_callback, game_manager);

    view_holder = view_holder_alloc();
    FURI_LOG_I("DEADZONE", "Hey, attaching to gui");

    view_holder_attach_to_gui(view_holder, gui);
    view_holder_set_view(view_holder, submenu_get_view(submenu));
    FURI_LOG_I("DEADZONE", "Hey, setting the view");
    if(reopen) {
        game_manager_game_stop(game_manager);
    }
    api_lock_wait_unlock(game_menu_exit_lock);
    FURI_LOG_I("DEADZONE", "Hey, done waiting!");

    view_holder_set_view(view_holder, NULL);

    // Delete everything to prevent memory leaks.
    view_holder_free(view_holder);
    submenu_free(submenu);
    // End access to the GUI API.
    furi_record_close(RECORD_GUI);
    FURI_LOG_I("DEADZONE", "Hey, destroyed view!");
}
