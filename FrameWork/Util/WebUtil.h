#pragma once
#include <Siv3D.hpp>

class WebUtil {
  public:
  WebUtil() = delete;

  static void PostToTwitter(StringView text,
                            const std::vector<StringView>& hash_tags = {}) {
    constexpr StringView kHeadURL = U"https://x.com/intent/post?text=";
    PostToSNS(kHeadURL, text, hash_tags);
  }

  static void PostToBlueSky(StringView text,
                            const std::vector<StringView>& hash_tags = {}) {
    constexpr StringView kHeadURL = U"https://bsky.app/intent/compose?text=";
    PostToSNS(kHeadURL, text, hash_tags);
  }

  private:
  static void PostToSNS(StringView head, StringView text,
                        const std::vector<StringView>& hash_tags) {
    String hash_tags_str = U"\n";
    for (StringView tag : hash_tags) {
      String hash_tag = String(tag);
      if (hash_tag.empty()) {
        continue;
      }
      if (hash_tag.front() != U'#') {
        hash_tag = U"#" + hash_tag;
      }
      hash_tags_str += hash_tag;
      hash_tags_str += U" ";
    }

    String url = String(head) + String(text);
    String siv_hash_tag_str = PercentEncode(hash_tags_str);
    System::LaunchBrowser(url + siv_hash_tag_str);
  }
};
