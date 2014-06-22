#include "as.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

GtkTextIter previ;
GtkWidget *main_view, *as_view;
GtkTextBuffer *main_buffer, *as_buffer;
int kf, vf;
gboolean use_cache = FALSE;
char line[BUFFER_SIZE];
struct Key_Head khead;
struct Key_Block kblock, cache[BCTN];
struct Value_Block vblock;
char value[AS_NUMBER][WORD_MAX_LENGTH + 1];
int seek_times[3], sn[AS_NUMBER], sp[5];
guint response_keys[AS_NUMBER] = {GDK_KEY_F1, GDK_KEY_F2, GDK_KEY_F3, GDK_KEY_F4, GDK_KEY_F5, GDK_KEY_F6, GDK_KEY_F7, GDK_KEY_F8};

void
destroy(GtkWidget *widget, gpointer data) {
	close(kf);
	close(vf);
	gtk_main_quit();
}

gboolean
Key_Control(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	int64_t i;

	if (event->keyval == GDK_KEY_Escape) {
		destroy(widget, data);
	} else {
		for (i = 0; i < AS_NUMBER && event->keyval != response_keys[i]; ++i);
		if (i < AS_NUMBER && value[i][0]) {
			main_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_view));
			gtk_text_buffer_insert_at_cursor(main_buffer, value[i], -1);
		}
	}
	return FALSE;
}

int
initialize(char *argv[]) {
	int64_t i;

	if ((kf = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to read key file \"%s\"\n", argv[1]);
		return -1;
	}
	if ((vf = open(argv[2], O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to read value file \"%s\"\n", argv[2]);
		return -1;
	}
	lseek64(kf, 0, SEEK_SET);
	if (read(kf, &khead, HEAD_SIZE) != HEAD_SIZE) {
		fprintf(stderr, "Failed to read head from key file \"%s\"\n", argv[1]);
		return -1;
	}
	if (khead.bctn != BCTN) {
		fprintf(stderr, "Error number of trees %"PRId64" instead of %d\n", khead.bctn, BCTN);
		return -1;
	}
	if (use_cache)
		for (i = 0; i < BCTN; ++i) {
			lseek64(kf, khead.bctinfo[i].start, SEEK_SET);
			if (read(kf, &cache[i], KEY_BLK_SIZE) != KEY_BLK_SIZE) {
				fprintf(stderr, "Failed to read cache %"PRId64" from key file \"%s\"\n", i, argv[1]);
				return -1;
			}
		}
	return 0;
}

int64_t
get_value(char *key, int skip) {
	unsigned hash = BKDRHash(key);
	int64_t cur, f, t, mid;
	int cmp_res;

	lseek64(kf, khead.bctinfo[hash].start, SEEK_SET);
	for (cur = 0; ; ) {
		if (!use_cache || cur >= KEY_ELEM_NUM) {
			read(kf, &kblock, KEY_BLK_SIZE);
			++seek_times[skip];
		}
		for (f = 0, t = ASMIN(KEY_ELEM_NUM, khead.bctinfo[hash].itemn - cur) - 1; f < t; ) {
			mid = f + ((t - f + 1) >> 1);
			if (strcmp(use_cache && cur < KEY_ELEM_NUM ? cache[hash].elems[mid].key : kblock.elems[mid].key, key) > 0)
				t = mid - 1;
			else
				f = mid;
		}
		cmp_res = strcmp(use_cache && cur < KEY_ELEM_NUM ? cache[hash].elems[f].key : kblock.elems[f].key, key);
		if (cmp_res == 0)
			return use_cache && cur < KEY_ELEM_NUM ? cache[hash].elems[f].value : kblock.elems[f].value;
		else if (cmp_res > 0 || (use_cache && cur < KEY_ELEM_NUM ? cache[hash].elems[f].child : kblock.elems[f].child) == -1)
			break;
		else {
			lseek64(kf, use_cache && cur < KEY_ELEM_NUM ? cache[hash].elems[f].child : kblock.elems[f].child, SEEK_SET);
			cur = (cur + 1) * KEY_ELEM_NUM;
		}
	}
	return -1;
}

void
get_result(char *key, int skip) {
	int i, j;
	char cval[AS_NUMBER][WORD_MAX_LENGTH + 1];
	int64_t val_off;

	for (i = skip + 1; i < sp[0]; ++i)
		if (sp[i + 1] - 1 - sp[i] > WORD_MAX_LENGTH)
			return;
	if ((val_off = get_value(key + sp[skip + 1] + 1, skip)) != -1) {
		lseek64(vf, val_off, SEEK_SET);
		if (read(vf, cval, VALUE_ELEM_SIZE) != VALUE_ELEM_SIZE) {
			fprintf(stderr, "Failed to read from value file\n");
			return;
		}
		++seek_times[skip];
		for (i = 0; i < AS_NUMBER && cval[i][0]; ++i) {
			for (j = 0; j < AS_NUMBER && value[j][0]; ++j)
				if (!strcmp(value[j], cval[i]))
					break;
			if (j < AS_NUMBER && !value[j][0]) {
				strcpy(value[j], cval[i]);
				sn[j] = skip;
			}
		}
	}
}

void
update_as(char *key) {
	int64_t i, j, len;
	GtkTextIter asi, aei;

	as_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(as_view));
	gtk_text_buffer_get_start_iter(as_buffer, &asi);
	gtk_text_buffer_get_end_iter(as_buffer, &aei);
	gtk_text_buffer_delete(as_buffer, &asi, &aei);

	memset(value, 0, sizeof(value));
	memset(seek_times, 0, sizeof(seek_times));
	for (i = 0, sp[sp[0] = 1] = -1; key[i]; ++i)
		if (key[i] == ' ')
			sp[++sp[0]] = i;
	sp[++sp[0]] = i;
	for (i = 0; i < sp[0] - 1 && !value[AS_NUMBER - 1][0]; ++i)
		get_result(key, i);

	len = sprintf(line, "Suggestions for ");
	line[len] = 0;
	gtk_text_buffer_get_end_iter(as_buffer, &aei);
	gtk_text_buffer_insert(as_buffer, &aei, line, len);
	len = sprintf(line, "%s", key);
	line[len] = 0;
	gtk_text_buffer_get_end_iter(as_buffer, &aei);
	gtk_text_buffer_insert_with_tags_by_name(as_buffer, &aei, line, len, "as_key_whole", NULL);
	gtk_text_buffer_get_end_iter(as_buffer, &aei);
	gtk_text_buffer_insert(as_buffer, &aei, ":\n", 2);
	for (i = 0; i < AS_NUMBER; ++i) {
		if (value[i][0]) {
			len = sprintf(line, "%2"PRId64". ", i + 1);
			for (j = 0; j <= sp[sn[i] + 1]; ++j)
				line[len++] = key[j];
			line[len] = 0;
			gtk_text_buffer_get_end_iter(as_buffer, &aei);
			gtk_text_buffer_insert(as_buffer, &aei, line, len);
			len = sprintf(line, "%s", key + sp[sn[i] + 1] + 1);
			line[len] = 0;
			gtk_text_buffer_get_end_iter(as_buffer, &aei);
			gtk_text_buffer_insert_with_tags_by_name(as_buffer, &aei, line, len, "as_key_real", NULL);
			len = sprintf(line, "%s", value[i]);
			line[len] = 0;
			gtk_text_buffer_get_end_iter(as_buffer, &aei);
			gtk_text_buffer_insert_with_tags_by_name(as_buffer, &aei, line, len, "as_value", NULL);
		}
		gtk_text_buffer_get_end_iter(as_buffer, &aei);
		gtk_text_buffer_insert(as_buffer, &aei, "\n", 1);
	}
}

void
update_statusbar(GtkTextBuffer *buffer, GtkStatusbar *statusbar) {
	gchar *msg, *key;
	gunichar cch;
	gint row, col, nspace;
	GtkTextIter iter, si, ei;

	gtk_statusbar_pop(statusbar, 0);
	gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
	if (!gtk_text_iter_equal(&iter, &previ)) {
		gtk_text_buffer_get_start_iter(buffer, &si);
		gtk_text_buffer_get_end_iter(buffer, &ei);
		gtk_text_buffer_remove_tag_by_name(buffer, "sel_key", &si, &ei);
		for (ei = iter, nspace = cch = 0; nspace < 3 && !gtk_text_iter_equal(&ei, &si); ) {
			gtk_text_iter_backward_char(&ei);
			cch = gtk_text_iter_get_char(&ei);
			if (cch == ' ')
				++nspace;
			else if (cch == '\n')
				break;
		}
		if (cch == ' ' || cch == '\n')
			gtk_text_iter_forward_char(&ei);
		while (gtk_text_iter_get_char(&ei) == ' ' && !gtk_text_iter_equal(&ei, &iter))
			gtk_text_iter_forward_char(&ei);
		key = gtk_text_iter_get_text(&ei, &iter);
		update_as(key);
		g_free(key);
		gtk_text_buffer_apply_tag_by_name(buffer, "sel_key", &ei, &iter);
		previ = iter;
	}
	row = gtk_text_iter_get_line(&iter);
	col = gtk_text_iter_get_line_offset(&iter);
	msg = g_strdup_printf("Col %d Ln %d\t\t\tseek times = %d + %d + %d = %d", col + 1, row + 1, seek_times[0], seek_times[1], seek_times[2], seek_times[0] + seek_times[1] + seek_times[2]);
	gtk_statusbar_push(statusbar, 0, msg);
	g_free(msg);
}

static void
mark_set_callback(GtkTextBuffer *buffer, const GtkTextIter *new_location, GtkTextMark *mark, gpointer data) {
	update_statusbar(buffer, GTK_STATUSBAR(data));
}


int
main(int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *box;
	GdkColor fcolor, bcolor;

	GtkWidget *statusbar;

	if (argc != 3) {
		fprintf(stderr, "Usage: ./as_show [key_file_name] [value_file_name]\n");
		return -1;
	}
	if (initialize(argv))
		return -1;
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 960, 640);
	gtk_window_set_title(GTK_WINDOW(window), "lines & cols");
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(box), FALSE);
	gtk_container_add(GTK_CONTAINER(window), box);

	main_view = gtk_text_view_new();
	gdk_color_parse("#00AEEF", &fcolor);
	gdk_color_parse("#FFFFFF", &bcolor);
	gtk_widget_modify_fg(GTK_WIDGET(main_view), GTK_STATE_NORMAL, &fcolor);
	gtk_widget_modify_bg(GTK_WIDGET(main_view), GTK_STATE_NORMAL, &bcolor);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(main_view), GTK_WRAP_WORD);
	gtk_widget_modify_font(GTK_WIDGET(main_view), pango_font_description_from_string("Droid Sans Mono Bold 15"));
	main_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_view));
	gtk_text_buffer_get_iter_at_mark(main_buffer, &previ, gtk_text_buffer_get_insert(main_buffer));
	gdk_color_parse("#FFFFFF", &fcolor);
	gdk_color_parse("#777777", &bcolor);
	gtk_text_buffer_create_tag(main_buffer, "sel_key", "foreground-gdk", &fcolor, "background-gdk", &bcolor, NULL);
	gtk_box_pack_start(GTK_BOX(box), main_view, TRUE, TRUE, 0);
	gtk_widget_grab_focus(main_view);

	statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(box), statusbar, FALSE, FALSE, 0);

	as_view = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(as_view), FALSE);
	gdk_color_parse("#FFFFFF", &fcolor);
	gdk_color_parse("#00AEEF", &bcolor);
	gtk_widget_modify_fg(GTK_WIDGET(as_view), GTK_STATE_NORMAL, &fcolor);
	gtk_widget_modify_bg(GTK_WIDGET(as_view), GTK_STATE_NORMAL, &bcolor);
	gtk_widget_modify_font(GTK_WIDGET(as_view), pango_font_description_from_string("Droid Sans Mono Bold 15"));
	as_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(as_view));
	gdk_color_parse("#FFBE00", &fcolor);
	gdk_color_parse("#FFFFFF", &bcolor);
	gtk_text_buffer_create_tag(as_buffer, "as_key_whole", "foreground-gdk", &fcolor, "background-gdk", &bcolor, NULL);
	gdk_color_parse("#FFFFFF", &fcolor);
	gdk_color_parse("#FFBE00", &bcolor);
	gtk_text_buffer_create_tag(as_buffer, "as_key_real", "foreground-gdk", &fcolor, "background-gdk", &bcolor, NULL);
	gdk_color_parse("#00AEEF", &fcolor);
	gdk_color_parse("#FFFFFF", &bcolor);
	gtk_text_buffer_create_tag(as_buffer, "as_value", "foreground-gdk", &fcolor, "background-gdk", &bcolor, NULL);
	gtk_box_pack_start(GTK_BOX(box), as_view, TRUE, TRUE, 0);
	
	g_signal_connect(main_buffer, "changed", G_CALLBACK(update_statusbar), statusbar);
	g_signal_connect_object(main_buffer, "mark_set", G_CALLBACK(mark_set_callback), statusbar, 0);
	g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(Key_Control), NULL);
	g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
	gtk_widget_show_all(window);
	update_statusbar(main_buffer, GTK_STATUSBAR (statusbar));

	gtk_main();
	return 0;
}
