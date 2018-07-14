import { Component } from '@angular/core';
import { NavController } from 'ionic-angular';
import { Hotspot } from '@ionic-native/hotspot';
import { AndroidPermissions } from '@ionic-native/android-permissions';
import { ServiceProvider } from '../../providers/service/service';

@Component({
  selector: 'page-hotspot',
  templateUrl: 'hotspot.html'
})
export class HotspotPage {

  wifi: boolean;
  ssid: string = 'Default';
  mode: string = 'WPA_PSK';
  password: string = '12345678';
  status: string = '';
  ip: string = '192.168.4.1';
  bulbs = [];

  constructor(
    public navCtrl: NavController,
    private hotspot: Hotspot,
    private http: ServiceProvider,
    private permission: AndroidPermissions
  ) {
    this.hotspot.isAvailable().then((resolve) => {
      this.status += "<br>~Available";
    }).catch((error) => {
      this.status += "<br>~Not Available";
    });

    this.hotspot.isWifiSupported().then((resolve) => {
      this.status += "<br>~Wifi Supported";

      this.hotspot.isWifiOn().then((resolve) => {
        this.wifi = resolve;
      }).catch((error) => {
        this.status += "<br>~" + error;
      });
    }).catch((error) => {
      this.status += "<br>~Wifi Not Supported";
    });


  }

  createHotspot() {
    this.permission.checkPermission(this.permission.PERMISSION.WRITE_SETTINGS).then((res) => {
      this.hotspot.createHotspot(this.ssid, this.mode, this.password).then((resolve) => {
        this.status += "<br>~Success";
      }).catch((error) => {
        this.status += "<br>~" + error;
      });
    }).catch((err) => {
      this.permission.requestPermission(this.permission.PERMISSION.WRITE_SETTINGS).then((resolve) => {

      }).catch((error) => {

      });
    });
  }

  getData() {
    this.http.get("http://" + this.ip).subscribe((data) => {
      this.status += "<br><br>~" + data.toString();
    });
  }

  toggleWifi() {
    this.hotspot.toggleWifi().then((resolve) => {
      this.wifi = !this.wifi;
    }).catch((error) => {
      this.status += "<br>~" + error;
    });
  }

}
