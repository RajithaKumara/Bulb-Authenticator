import { Component } from '@angular/core';
import { NavController, ToastController, Platform } from 'ionic-angular';
import { Hotspot } from '@ionic-native/hotspot';
import { NativeStorage } from '@ionic-native/native-storage';
import { ServiceProvider } from '../../providers/service/service'
import { AlertController } from 'ionic-angular';
import { LoadingController } from 'ionic-angular';
import { Network } from '@ionic-native/network';

@Component({
  selector: 'page-hotspot',
  templateUrl: 'hotspot.html'
})
export class HotspotPage {
  connected = false;
  wifi: boolean;
  ssid: string = 'Bulb-AP';
  password: string = '12345678';
  status: string = '';
  ip: string = '192.168.4.1';
  bulbs = [];
  object: any;
  navigator: any;
  Connection: any;
  errors: string = '';
  passwordType: string = 'password';
  passwordIcon: string = 'eye-off';
  constructor(
    private alertCtrl: AlertController,
    public navCtrl: NavController,
    public storage: NativeStorage,
    private hotspot: Hotspot,
    private http: ServiceProvider,
    public toastCtrl: ToastController,
    public loadingCtrl: LoadingController,
    private network: Network,
    private platform: Platform,

  ) {


    this.hotspot.isAvailable().then((resolve) => {
      this.status += "<br>~Available";
    }).catch((error) => {
      this.status += "<br>~Not Available";
      this.errors += error;
    });

    this.hotspot.isWifiSupported().then((resolve) => {
      this.status += "<br>~Wifi Supported";

      this.hotspot.isWifiOn().then((resolve) => {
        this.wifi = resolve;
      }).catch((error) => {
        this.status += "<br>~" + error;
        this.errors += error;
      });
    }).catch((error) => {
      this.status += "<br>~Wifi Not Supported";
      this.errors += error;
    });

    if (this.errors.length > 0) {
      this.showAlert("Wifi Error", this.errors);
    }
  }
  connectTobulb() {
    this.status += "<br>~ip=" + this.ip;
    this.status += "<br>~ssid=" + this.ssid;
    this.status += "<br>~passwd=" + this.password;
    // this.connected=true;
    // this.storage.setItem('ip', this.ip);
    this.presentLoadig();
    this.hotspot.connectToWifi(this.ssid, this.password).then((resolve) => {
      this.status += "<br>~Success";
      this.connected = true;
    }).catch((error) => {
      this.status += "<br>~" + error;
      this.showAlert("Connect error", "please check your credentials");
    });
  }

  updatebulbid() {

    let alert = this.alertCtrl.create({
      title: 'update bulb',
      inputs: [
        {
          name: 'id',
          placeholder: 'Bulb id',
          max: 2
        }

      ],
      buttons: [
        {
          text: 'Cancel',
          role: 'cancel',
          handler: data => {
            console.log('Cancel clicked');
          }
        },
        {
          text: 'update',
          handler: data => {


            this.status += "<br>~id=" + data.id;
            this.http.get("http://" + this.ip + "/changeId?id=" + data.id).subscribe((observer) => {

            }, (err) => {
              this.status += "<br>~can not change id" + err;
              this.showAlert("connection error", "can not update bulb id " + err);
            }, () => {
              this.status += "<br>~bulb id has changed";
            });


          }
        }
      ]
    });
    alert.present();
  }

  updatepassword() {

    this.storage.getItem('password').then((result) => {
      this.status += "<br>~password stored=" + result;
      this.http.get("http://" + this.ip + "/changePass?password=" + result).subscribe((observer) => {
        this.status += "<br>~password " + observer;
      }, (err) => {
        this.status += "<br>~can not change password" + err;
        this.showAlert("connection error", "can not update password " + err);
      }, () => {
        this.status += "<br>~password has changed";
      });
    }).catch((error) => {
      this.showToast(error.message);
    });
  }

  showAlert(Title, Mess) {
    const alert = this.alertCtrl.create({

      title: Title,
      subTitle: Mess,
      buttons: ['OK']
    });
    alert.present();
  }

  presentLoadig() {
    let loading = this.loadingCtrl.create({
      content: 'Please wait...'
    });

    loading.present();

    setTimeout(() => {
      loading.dismiss();
    }, 1000);
  }

  // addNewBulb() {
  //   let bulbId = 1;
  //   this.http.get("http://" + this.ip + "/addBulb?id=" + bulbId)
  //     .subscribe((observer) => {
  //       this.object = observer;
  //     }, (error) => {
  //       // console.log('api error', error);
  //       this.status += "<br><br>~E~" + error;
  //       this.object = error;
  //     }, () => {
  //       // console.log('complete');
  //       this.status += "<br><br>~complete";
  //     }
  //     );
  // }

  showToast(msg) {
    const toast = this.toastCtrl.create({
      message: msg,
      duration: 3000
    });
    toast.present();
  }

  getpassword() {
    this.status += "<br>~network=" + this.network.type;
    this.status += "<br>~ip=" + this.ip;
    this.status += "<br>~ssid=" + this.ssid;
    this.status += "<br>~passwd=" + this.password;
    this.storage.getItem('password').then((result) => {
      this.status += "<br>~password stored=" + result;
    }).catch((error) => {
      this.showToast(error.message);
    });
  }

  hideShowPassword() {

    this.passwordType = this.passwordType === 'text' ? 'password' : 'text';
    this.passwordIcon = this.passwordIcon === 'eye-off' ? 'eye' : 'eye-off';
  }

  updateFinish() {
    this.http.get("http://" + this.ip + "/finish")
      .subscribe((observer) => {
        this.object = observer;
        this.showToast("Successfully finished...");
      }, (error) => {
        this.status += "<br><br>~E~" + error;
        this.object = error;
      }, () => {
        this.status += "<br><br>~complete";
      }
      );
  }

  enableQuickSwitchOnMode(){
    this.http.get("http://" + this.ip + "/quickSwitchOn?state=true")
      .subscribe((observer) => {
        this.object = observer;
        this.showToast("Successfully enabled...");
      }, (error) => {
        this.status += "<br><br>~E~" + error;
        this.object = error;
      }, () => {
        this.status += "<br><br>~complete";
      }
      );
  }

  disableQuickSwitchOnMode(){
    this.http.get("http://" + this.ip + "/quickSwitchOn?state=false")
      .subscribe((observer) => {
        this.object = observer;
        this.showToast("Successfully disabled...");
      }, (error) => {
        this.status += "<br><br>~E~" + error;
        this.object = error;
      }, () => {
        this.status += "<br><br>~complete";
      }
      );
  }

}
