import { Component } from '@angular/core';
import { NavController } from 'ionic-angular';
import { Hotspot } from '@ionic-native/hotspot';
import { NativeStorage } from '@ionic-native/native-storage';
import { ServiceProvider } from '../../providers/service/service';
// import {BulbNamePipe} from '../../pipes/bulb-name/bulb-name';

@Component({
  selector: 'page-wifi',
  templateUrl: 'wifi.html'
})
export class WiFiPage {

  wifi: boolean;
  ssid: string = 'Default';
  password: string = '12345678';
  status: string = '';
  ip: string = '192.168.4.1';
  bulbs = [];
  object: any;
  saveMode:boolean = false;

  constructor(
    public navCtrl: NavController,
    public storage: NativeStorage,
    private hotspot: Hotspot,
    private http: ServiceProvider
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

  connectToWifi() {
    this.storage.setItem('ip', this.ip);
    this.hotspot.connectToWifi(this.ssid, this.password).then((resolve) => {
      this.status += "<br>~Success";
    }).catch((error) => {
      this.status += "<br>~" + error;
    });
  }

  getDetails() {
    this.http.get("http://" + this.ip + "/getDetails")
      .subscribe((observer) => {
        // console.log('api success', observer);
        // this.status += "<br><br>~" + observer;
        this.object = observer;

        let obj = [
          { "id": 1, "intensity": 1000, "state": 1 },
          { "id": 2, "intensity": 1030, "state": 0 },
          { "id": 3, "intensity": 1070, "state": 1 },
        ];
        this.bulbs = obj;
        this.object = obj;
      }, (error) => {
        // console.log('api error', error);
        this.status += "<br><br>~E~" + error;
        this.object = error;
      }, () => {
        // console.log('complete');
        this.status += "<br><br>~complete";
      }
      );
  }

  addNewBulb() {

  }

  toggleWifi() {
    this.hotspot.toggleWifi().then((resolve) => {
      this.wifi = !this.wifi;
    }).catch((error) => {
      this.status += "<br>~" + error;
    });
  }

}
