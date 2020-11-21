/*
 * Copyright 2013–2020 Kullo GmbH
 *
 * This source code is licensed under the 3-clause BSD license. See LICENSE.txt
 * in the root directory of this source tree for details.
 */
import QtQuick 2.4

// 1F600–1F64F, i.e. 5*16 = 80 code points
// 2 elements deactivated because not supported on Ubuntu 15.10 (Unicode 8.0)
// 2 elements deactivated because in Unicode 7.0
// => 76 elements
// See e.g. http://apps.timwhitlock.info/emoji/tables/unicode (incomplete list)

ListModel {
    ListElement { emoji_: "\uD83D\uDE00"; description_: qsTr("grinning face", "emoji description") /* age: 6.1 */                                        }
    ListElement { emoji_: "\uD83D\uDE01"; description_: qsTr("grinning face with smiling eyes", "emoji description") /* age: 6.0 */                      }
    ListElement { emoji_: "\uD83D\uDE02"; description_: qsTr("face with tears of joy", "emoji description") /* age: 6.0 */                               }
    ListElement { emoji_: "\uD83D\uDE03"; description_: qsTr("smiling face with open mouth", "emoji description") /* age: 6.0 */                         }
    ListElement { emoji_: "\uD83D\uDE04"; description_: qsTr("smiling face with open mouth and smiling eyes", "emoji description") /* age: 6.0 */        }
    ListElement { emoji_: "\uD83D\uDE05"; description_: qsTr("smiling face with open mouth and cold sweat", "emoji description") /* age: 6.0 */          }
    ListElement { emoji_: "\uD83D\uDE06"; description_: qsTr("smiling face with open mouth and tightly-closed eyes", "emoji description") /* age: 6.0 */ }
    ListElement { emoji_: "\uD83D\uDE07"; description_: qsTr("smiling face with halo", "emoji description") /* age: 6.0 */                               }
    ListElement { emoji_: "\uD83D\uDE08"; description_: qsTr("smiling face with horns", "emoji description") /* age: 6.0 */                              }
    ListElement { emoji_: "\uD83D\uDE09"; description_: qsTr("winking face", "emoji description") /* age: 6.0 */                                         }
    ListElement { emoji_: "\uD83D\uDE0A"; description_: qsTr("smiling face with smiling eyes", "emoji description") /* age: 6.0 */                       }
    ListElement { emoji_: "\uD83D\uDE0B"; description_: qsTr("face savouring delicious food", "emoji description") /* age: 6.0 */                        }
    ListElement { emoji_: "\uD83D\uDE0C"; description_: qsTr("relieved face", "emoji description") /* age: 6.0 */                                        }
    ListElement { emoji_: "\uD83D\uDE0D"; description_: qsTr("smiling face with heart-shaped eyes", "emoji description") /* age: 6.0 */                  }
    ListElement { emoji_: "\uD83D\uDE0E"; description_: qsTr("smiling face with sunglasses", "emoji description") /* age: 6.0 */                         }
    ListElement { emoji_: "\uD83D\uDE0F"; description_: qsTr("smirking face", "emoji description") /* age: 6.0 */                                        }
    ListElement { emoji_: "\uD83D\uDE10"; description_: qsTr("neutral face", "emoji description") /* age: 6.0 */                                         }
    ListElement { emoji_: "\uD83D\uDE11"; description_: qsTr("expressionless face", "emoji description") /* age: 6.1 */                                  }
    ListElement { emoji_: "\uD83D\uDE12"; description_: qsTr("unamused face", "emoji description") /* age: 6.0 */                                        }
    ListElement { emoji_: "\uD83D\uDE13"; description_: qsTr("face with cold sweat", "emoji description") /* age: 6.0 */                                 }
    ListElement { emoji_: "\uD83D\uDE14"; description_: qsTr("pensive face", "emoji description") /* age: 6.0 */                                         }
    ListElement { emoji_: "\uD83D\uDE15"; description_: qsTr("confused face", "emoji description") /* age: 6.1 */                                        }
    ListElement { emoji_: "\uD83D\uDE16"; description_: qsTr("confounded face", "emoji description") /* age: 6.0 */                                      }
    ListElement { emoji_: "\uD83D\uDE17"; description_: qsTr("kissing face", "emoji description") /* age: 6.1 */                                         }
    ListElement { emoji_: "\uD83D\uDE18"; description_: qsTr("face throwing a kiss", "emoji description") /* age: 6.0 */                                 }
    ListElement { emoji_: "\uD83D\uDE19"; description_: qsTr("kissing face with smiling eyes", "emoji description") /* age: 6.1 */                       }
    ListElement { emoji_: "\uD83D\uDE1A"; description_: qsTr("kissing face with closed eyes", "emoji description") /* age: 6.0 */                        }
    ListElement { emoji_: "\uD83D\uDE1B"; description_: qsTr("face with stuck-out tongue", "emoji description") /* age: 6.1 */                           }
    ListElement { emoji_: "\uD83D\uDE1C"; description_: qsTr("face with stuck-out tongue and winking eye", "emoji description") /* age: 6.0 */           }
    ListElement { emoji_: "\uD83D\uDE1D"; description_: qsTr("face with stuck-out tongue and tightly-closed eyes", "emoji description") /* age: 6.0 */   }
    ListElement { emoji_: "\uD83D\uDE1E"; description_: qsTr("disappointed face", "emoji description") /* age: 6.0 */                                    }
    ListElement { emoji_: "\uD83D\uDE1F"; description_: qsTr("worried face", "emoji description") /* age: 6.1 */                                         }
    ListElement { emoji_: "\uD83D\uDE20"; description_: qsTr("angry face", "emoji description") /* age: 6.0 */                                           }
    ListElement { emoji_: "\uD83D\uDE21"; description_: qsTr("pouting face", "emoji description") /* age: 6.0 */                                         }
    ListElement { emoji_: "\uD83D\uDE22"; description_: qsTr("crying face", "emoji description") /* age: 6.0 */                                          }
    ListElement { emoji_: "\uD83D\uDE23"; description_: qsTr("persevering face", "emoji description") /* age: 6.0 */                                     }
    ListElement { emoji_: "\uD83D\uDE24"; description_: qsTr("face with look of triumph", "emoji description") /* age: 6.0 */                            }
    ListElement { emoji_: "\uD83D\uDE25"; description_: qsTr("disappointed but relieved face", "emoji description") /* age: 6.0 */                       }
    ListElement { emoji_: "\uD83D\uDE26"; description_: qsTr("frowning face with open mouth", "emoji description") /* age: 6.1 */                        }
    ListElement { emoji_: "\uD83D\uDE27"; description_: qsTr("anguished face", "emoji description") /* age: 6.1 */                                       }
    ListElement { emoji_: "\uD83D\uDE28"; description_: qsTr("fearful face", "emoji description") /* age: 6.0 */                                         }
    ListElement { emoji_: "\uD83D\uDE29"; description_: qsTr("weary face", "emoji description") /* age: 6.0 */                                           }
    ListElement { emoji_: "\uD83D\uDE2A"; description_: qsTr("sleepy face", "emoji description") /* age: 6.0 */                                          }
    ListElement { emoji_: "\uD83D\uDE2B"; description_: qsTr("tired face", "emoji description") /* age: 6.0 */                                           }
    ListElement { emoji_: "\uD83D\uDE2C"; description_: qsTr("grimacing face", "emoji description") /* age: 6.1 */                                       }
    ListElement { emoji_: "\uD83D\uDE2D"; description_: qsTr("loudly crying face", "emoji description") /* age: 6.0 */                                   }
    ListElement { emoji_: "\uD83D\uDE2E"; description_: qsTr("face with open mouth", "emoji description") /* age: 6.1 */                                 }
    ListElement { emoji_: "\uD83D\uDE2F"; description_: qsTr("hushed face", "emoji description") /* age: 6.1 */                                          }
    ListElement { emoji_: "\uD83D\uDE30"; description_: qsTr("face with open mouth and cold sweat", "emoji description") /* age: 6.0 */                  }
    ListElement { emoji_: "\uD83D\uDE31"; description_: qsTr("face screaming in fear", "emoji description") /* age: 6.0 */                               }
    ListElement { emoji_: "\uD83D\uDE32"; description_: qsTr("astonished face", "emoji description") /* age: 6.0 */                                      }
    ListElement { emoji_: "\uD83D\uDE33"; description_: qsTr("flushed face", "emoji description") /* age: 6.0 */                                         }
    ListElement { emoji_: "\uD83D\uDE34"; description_: qsTr("sleeping face", "emoji description") /* age: 6.1 */                                        }
    ListElement { emoji_: "\uD83D\uDE35"; description_: qsTr("dizzy face", "emoji description") /* age: 6.0 */                                           }
    ListElement { emoji_: "\uD83D\uDE36"; description_: qsTr("face without mouth", "emoji description") /* age: 6.0 */                                   }
    ListElement { emoji_: "\uD83D\uDE37"; description_: qsTr("face with medical mask", "emoji description") /* age: 6.0 */                               }
    ListElement { emoji_: "\uD83D\uDE38"; description_: qsTr("grinning cat face with smiling eyes", "emoji description") /* age: 6.0 */                  }
    ListElement { emoji_: "\uD83D\uDE39"; description_: qsTr("cat face with tears of joy", "emoji description") /* age: 6.0 */                           }
    ListElement { emoji_: "\uD83D\uDE3A"; description_: qsTr("smiling cat face with open mouth", "emoji description") /* age: 6.0 */                     }
    ListElement { emoji_: "\uD83D\uDE3B"; description_: qsTr("smiling cat face with heart-shaped eyes", "emoji description") /* age: 6.0 */              }
    ListElement { emoji_: "\uD83D\uDE3C"; description_: qsTr("cat face with wry smile", "emoji description") /* age: 6.0 */                              }
    ListElement { emoji_: "\uD83D\uDE3D"; description_: qsTr("kissing cat face with closed eyes", "emoji description") /* age: 6.0 */                    }
    ListElement { emoji_: "\uD83D\uDE3E"; description_: qsTr("pouting cat face", "emoji description") /* age: 6.0 */                                     }
    ListElement { emoji_: "\uD83D\uDE3F"; description_: qsTr("crying cat face", "emoji description") /* age: 6.0 */                                      }
    ListElement { emoji_: "\uD83D\uDE40"; description_: qsTr("weary cat face", "emoji description") /* age: 6.0 */                                       }
    ListElement { emoji_: "\uD83D\uDE45"; description_: qsTr("face with no good gesture", "emoji description") /* age: 6.0 */                            }
    ListElement { emoji_: "\uD83D\uDE46"; description_: qsTr("face with ok gesture", "emoji description") /* age: 6.0 */                                 }
    ListElement { emoji_: "\uD83D\uDE47"; description_: qsTr("person bowing deeply", "emoji description") /* age: 6.0 */                                 }
    ListElement { emoji_: "\uD83D\uDE48"; description_: qsTr("see-no-evil monkey", "emoji description") /* age: 6.0 */                                   }
    ListElement { emoji_: "\uD83D\uDE49"; description_: qsTr("hear-no-evil monkey", "emoji description") /* age: 6.0 */                                  }
    ListElement { emoji_: "\uD83D\uDE4A"; description_: qsTr("speak-no-evil monkey", "emoji description") /* age: 6.0 */                                 }
    ListElement { emoji_: "\uD83D\uDE4B"; description_: qsTr("happy person raising one hand", "emoji description") /* age: 6.0 */                        }
    ListElement { emoji_: "\uD83D\uDE4C"; description_: qsTr("person raising both hands in celebration", "emoji description") /* age: 6.0 */             }
    ListElement { emoji_: "\uD83D\uDE4D"; description_: qsTr("person frowning", "emoji description") /* age: 6.0 */                                      }
    ListElement { emoji_: "\uD83D\uDE4E"; description_: qsTr("person with pouting face", "emoji description") /* age: 6.0 */                             }
    ListElement { emoji_: "\uD83D\uDE4F"; description_: qsTr("person with folded hands", "emoji description") /* age: 6.0 */                             }
}
