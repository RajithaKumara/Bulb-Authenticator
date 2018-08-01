import { Component } from '@angular/core';
import { NavController, ToastController } from 'ionic-angular';
import { NativeStorage } from '@ionic-native/native-storage';
import { ServiceProvider } from '../../providers/service/service';

@Component({
  selector: 'page-bulb',
  templateUrl: 'bulb.html'
})
export class BulbPage {

  add: boolean = false;
  bulbName: string = '';
  bulbId: string = '';
  bulbData: string = '';
  ip: string;

  status: string = '';

  bulbKeys = [];
  bulbs = [];
  object: any;

  constructor(
    public navCtrl: NavController,
    public storage: NativeStorage,
    public toastCtrl: ToastController,
    private http: ServiceProvider
  ) {
    this.storage.getItem('ip').then((result) => {
      this.ip = result;
    }).catch((error) => {
      this.showToast('Error occured when loading ip...');
    });

    this.getBulbs();
  }

  addABulb() {
    this.add = !this.add;
  }

  addBulb() {
    this.http.get("http://" + this.ip + "/addBulb?id=" + this.bulbId)
      .subscribe((observer) => {
        this.object = observer;
      }, (error) => {
        this.showToast('Error occured in request...');
      }, () => {
        //Store the bulb data in mobile
        this.storage.setItem("bulbID-" + this.bulbId, {
          bulbName: this.bulbName,
          bulbId: this.bulbId,
          bulbData: this.bulbData
        }).then((resolve) => {
          this.showToast('Successfully stored...');
          this.getBulbs();
          this.bulbName = ""
          this.bulbId = ""
          this.bulbData = ""
          this.add = false;
        }).catch((error) => {
          this.showToast('Error occured in storing...');
        });
      }
      );
  }

  getBulbs() {
    this.bulbKeys = [];
    this.bulbs = [];
    this.storage.keys().then((keys) => {
      this.bulbKeys = keys;
      this.status += '<br>' + keys;
      this.bulbKeys.forEach(id => {
        let strKey = "" + id;
        if (strKey.substring(0, 7) === "bulbID-") {
          this.storage.getItem(strKey).then((value) => {
            value["id"] = strKey.substring(7);
            this.bulbs.push(value);
          }).catch((error) => {
            this.showToast("Error occured...")
          });
        }
      });
    }).catch((error) => {
      this.status += '<br>' + error;
    });
  }

  removeBulb(bulb) {
    let id = bulb.id;
    this.http.get("http://" + this.ip + "/removeBulb?id=" + id)
      .subscribe((observer) => {
        this.object = observer;
      }, (error) => {
        this.showToast('Error occured in request...');
      }, () => {
        this.storage.remove("bulbID-" + id).then((resolve) => {
          this.showToast('Successfully removed...');
          this.getBulbs();
        }).catch((error) => {
          this.showToast('Error occured in storage...');
        });
      }
      );

  }

  showToast(msg) {
    const toast = this.toastCtrl.create({
      message: msg,
      duration: 3000
    });
    toast.present();
  }
}
