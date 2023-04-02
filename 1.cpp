#include "rawdrawandroid.h"
#include <jni.h>
#include <android/native_activity.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <stdlib.h>

// Декларируем функцию, которая будет вызываться из Java кода
extern "C" JNIEXPORT void JNICALL Java_com_example_rawdrawandroid_MainActivity_showWebView(JNIEnv* env, jobject thiz);

// Глобальные переменные
JNIEnv* jniEnv = NULL;
jobject activity = NULL;
jobject webView = NULL;
jmethodID loadUrlMethod = NULL;
jmethodID setLayoutParamsMethod = NULL;

// Функция инициализации
void init() {
    ANativeActivity* nativeActivity = GetNativeActivity();
    JavaVM* jvm = nativeActivity->vm;
    jvm->AttachCurrentThread(&jniEnv, NULL);

    activity = nativeActivity->clazz;

    // Находим класс WebView и получаем его конструктор
    jclass webViewClass = jniEnv->FindClass("android/webkit/WebView");
    jmethodID webViewConstructor = jniEnv->GetMethodID(webViewClass, "<init>", "(Landroid/content/Context;)V");

    // Создаем экземпляр WebView
    webView = jniEnv->NewObject(webViewClass, webViewConstructor, activity);

    // Находим методы WebView для загрузки URL и установки параметров макета
    loadUrlMethod = jniEnv->GetMethodID(webViewClass, "loadUrl", "(Ljava/lang/String;)V");
    jclass layoutParamsClass = jniEnv->FindClass("android/view/ViewGroup$LayoutParams");
    setLayoutParamsMethod = jniEnv->GetMethodID(webViewClass, "setLayoutParams", "(Landroid/view/ViewGroup$LayoutParams;)V");
    jmethodID layoutParamsConstructor = jniEnv->GetMethodID(layoutParamsClass, "<init>", "(II)V");

    // Создаем параметры макета для WebView
    jobject layoutParams = jniEnv->NewObject(layoutParamsClass, layoutParamsConstructor, -1, -1);
    jniEnv->CallVoidMethod(webView, setLayoutParamsMethod, layoutParams);
}

// Функция рисования
void draw() {
    // Рисуем фон
    set_color(255, 255, 255);
    fill_rect(0, 0, get_window_width(), get_window_height());

    // Загружаем URL в WebView
    jstring url = jniEnv->NewStringUTF("https://www.example.com/");
    jniEnv->CallVoidMethod(webView, loadUrlMethod, url);
}

// Функция обработки событий нажатия клавиш
void key_down(int keycode) {
    // Закрываем приложение при нажатии клавиши Back
    if (keycode == KEY_BACK) {
        exit(0);
    }
}

// Функция освобождения ресурсов
void cleanup() {
    jniEnv->DeleteLocalRef(webView);
    jniEnv->DeleteGlobalRef(activity);
    jniEnv->DeleteGlobalRef(loadUrlMethod);
    jniEnv->DeleteGlobalRef(setLayoutParamsMethod);
    jniEnv->DeleteGlobalRef(jniEnv);
}

// Функция, которая будет вызываться из Java кода для отображения WebView
extern "C" JNIEXPORT void JNICALL Java_com_example_rawdrawandroid_MainActivity_draw(JNIEnv *env, jobject thiz, jobject surface) {
// Получаем информацию о surface
ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
ANativeWindow_Buffer buffer;
if (ANativeWindow_lock(window, &buffer, NULL) < 0) {
// Ошибка при получении буфера
return;
}

// Рисуем фон
for (int i = 0; i < buffer.height; i++) {
    uint32_t *row = (uint32_t *)((char *)buffer.bits + i * buffer.stride * 4);
    for (int j = 0; j < buffer.width; j++) {
        row[j] = 0xFF000000; // черный цвет
    }
}

// Создаем WebView
JNIEnv* jniEnv;
JavaVM* javaVM = AndroidJNI::GetJavaVM();
JavaVMAttachArgs attachArgs = { JNI_VERSION_1_6, NULL, NULL };
jint attachResult = javaVM->AttachCurrentThread(&jniEnv, &attachArgs);
jobject activityObj = AndroidJNI::GetStaticObjectField(env, surfaceClass, surfaceActivityFieldID);
jclass activityClass = jniEnv->GetObjectClass(activityObj);
jmethodID getApplicationContextMethodID = jniEnv->GetMethodID(activityClass, "getApplicationContext", "()Landroid/content/Context;");
jobject contextObj = jniEnv->CallObjectMethod(activityObj, getApplicationContextMethodID);
jclass webViewClass = jniEnv->FindClass("android/webkit/WebView");
jmethodID webViewConstructorMethodID = jniEnv->GetMethodID(webViewClass, "<init>", "(Landroid/content/Context;)V");
jobject webViewObj = jniEnv->NewObject(webViewClass, webViewConstructorMethodID, contextObj);
jmethodID webViewLoadUrlMethodID = jniEnv->GetMethodID(webViewClass, "loadUrl", "(Ljava/lang/String;)V");
jstring urlStr = jniEnv->NewStringUTF("https://www.google.com/");
jniEnv->CallVoidMethod(webViewObj, webViewLoadUrlMethodID, urlStr);

// Отрисовываем WebView
jclass viewClass = jniEnv->FindClass("android/view/View");
jmethodID viewGetDisplayMethodID = jniEnv->GetMethodID(viewClass, "getDisplay", "()Landroid/view/Display;");
jobject displayObj = jniEnv->CallObjectMethod(surface, viewGetDisplayMethodID);
jclass displayClass = jniEnv->FindClass("android/view/Display");
jmethodID displayGetRealMetricsMethodID = jniEnv->GetMethodID(displayClass, "getRealMetrics", "(Landroid/util/DisplayMetrics;)V");
jobject displayMetricsObj = jniEnv->NewObject(jniEnv->FindClass("android/util/DisplayMetrics"), jniEnv->GetMethodID(jniEnv->FindClass("android/util/DisplayMetrics"), "<init>", "()V"));
jniEnv->CallVoidMethod(displayObj, displayGetRealMetricsMethodID, displayMetricsObj);
int width = jniEnv->CallIntMethod(displayMetricsObj, jniEnv->GetMethodID(jniEnv->FindClass("android/util/DisplayMetrics"), "widthPixels", "()I"));
int height = jniEnv->CallIntMethod(displayMetricsObj, jniEnv->GetMethodID(jniEnv->FindClass("android/util/DisplayMetrics"), "heightPixels", "()I"));
jobject canvas
