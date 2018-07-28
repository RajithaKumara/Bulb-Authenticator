import { Component } from '@angular/core';
import { NavController } from 'ionic-angular';
import { NativeStorage } from '@ionic-native/native-storage';
import { ToastController } from 'ionic-angular';
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
        // this.status += "<br><br>~E~" + error;
        // this.object = error;
      }, () => {
        //Store the bulb data in mobile
        let current = Date.now();
        this.storage.setItem("" + current, {
          bulbName: this.bulbName,
          bulbId: this.bulbId,
          bulbData: this.bulbData
        }).then((resolve) => {
          this.showToast('Successfully stored...');
          this.bulbName = ""
          this.bulbId = ""
          this.bulbData = ""
          this.add = false;
        }).catch((error) => {
          this.showToast('Error occured in storing...');
        });

        // this.status += "<br><br>~complete";
      }
      );


    // if (this.bulbKeys.length > 0) {
    //   this.getBulbs();
    // }
  }

  getBulbs() {
    this.bulbKeys = [];
    this.bulbs = [];
    this.storage.keys().then((keys) => {
      this.bulbKeys = keys;
      this.status += '<br>' + keys;
      this.bulbKeys.forEach(id => {
        this.storage.getItem("" + id).then((value) => {
          this.bulbs.push(value);
        }).catch((error) => {
          this.showToast("Error occured...")
        });
      });
    }).catch((error) => {
      this.status += '<br>' + error;
    });
  }

  addNewBulb() {
    let bulbId = 1;
    this.http.get("http://" + this.ip + "/addBulb?id=" + bulbId)
      .subscribe((observer) => {
        this.object = observer;
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

  showToast(msg) {
    const toast = this.toastCtrl.create({
      message: msg,
      duration: 3000
    });
    toast.present();
  }


}
