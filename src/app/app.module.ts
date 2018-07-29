import { HttpClientModule } from '@angular/common/http';
import { BrowserModule } from '@angular/platform-browser';
import { ErrorHandler, NgModule } from '@angular/core';
import { IonicApp, IonicErrorHandler, IonicModule } from 'ionic-angular';
import { SplashScreen } from '@ionic-native/splash-screen';
import { StatusBar } from '@ionic-native/status-bar';
import { Hotspot } from '@ionic-native/hotspot';
import { NativeStorage } from '@ionic-native/native-storage';
import { AndroidPermissions } from '@ionic-native/android-permissions';

import { MyApp } from './app.component';
import { HomePage } from '../pages/home/home';
import { BulbPage } from '../pages/bulb/bulb';
import { HotspotPage } from '../pages/hotspot/hotspot';
import { WiFiPage } from '../pages/wifi/wifi';
import { ServiceProvider } from '../providers/service/service';
import { PipesModule } from '../pipes/pipes.module';
import { Network } from '@ionic-native/network';



@NgModule({
  declarations: [
    MyApp,
    HomePage,
    BulbPage,
    HotspotPage,
    WiFiPage
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    PipesModule,
    IonicModule.forRoot(MyApp)
  ],
  bootstrap: [IonicApp],
  entryComponents: [
    MyApp,
    HomePage,
    BulbPage,
    HotspotPage,
    WiFiPage
  ],
  providers: [
    StatusBar,
    SplashScreen,
    Hotspot,
    NativeStorage,
    AndroidPermissions,
    { provide: ErrorHandler, useClass: IonicErrorHandler },
    ServiceProvider,
    Network
  ]
})
export class AppModule { }
