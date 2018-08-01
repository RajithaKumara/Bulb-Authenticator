import { Component } from '@angular/core';
import { NavController } from 'ionic-angular';
import { BulbPage } from '../bulb/bulb';
import { HotspotPage } from '../hotspot/hotspot';
import { WiFiPage } from '../wifi/wifi';

@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {

  constructor(
    public navCtrl: NavController
  ) {

  }

  pushHotspotPage() {
    return HotspotPage;
  }

  pushBulbPage() {
    return BulbPage;
  }

  pushWiFiPage() {
    return WiFiPage;
  }
}
